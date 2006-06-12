import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.PrintStream;
import java.net.InetAddress;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Arrays;
import java.util.List;
import java.util.Properties;

/** keeps the address in network order (MSB first) */
abstract class IPAddress {
	public int[] address;
	
	public static IPAddress createAddress(int[] in) {
		switch (in.length) {
			case IPv4Address.LENGTH:
				return new IPv4Address(in);
				
			case IPv6Address.LENGTH:
				return new IPv6Address(in);
				
			default:
				throw new RuntimeException("Invalid address length ("+(in.length*32)+" bits)");
		}
	}
	
	public String getHex(int limb) {
		return String.format("0x%x", address[limb]);
	}
}

class IPv4Address extends IPAddress {
	public static final int LENGTH = 1;
	
	public IPv4Address(int[] addr) {
		if (addr.length != 1) {
			throw new RuntimeException("Invalid length for IPv4 address (need: 32bit, got: "+(addr.length*32));
		}
		address = new int[1];
		address[0] = addr[0];
	}
	
	public String toString() {
		return Integer.toString(address[0]>>24)+"."+
			((address[0]>>16) & 0xff)+"."+
			((address[0]>>8) & 0xff)+"."+
			((address[0]) & 0xff);
	}
}

class IPv6Address extends IPAddress {
	public static final int LENGTH = 4;

	public IPv6Address(int[] addr) {
		if (addr.length != 1) {
			throw new RuntimeException("Invalid length for IPv6 address (need: 128bit, got: "+(addr.length*32));
		}
		address = new int[4];
		System.arraycopy(addr, 0, address, 0, 4);
	}
	
	public String toString() {
		ByteArrayOutputStream baos = new ByteArrayOutputStream(48);
		PrintStream ps = new PrintStream(baos);
		
		for (int i = 0; i < 3; i++) {
			ps.printf("%x:%x:", (address[i]>>16)&0xffff, address[i]&0xffff);
		}
		ps.printf("%x:%x", (address[3]>>16)&0xffff, address[3]&0xffff);
		ps.flush();
		return baos.toString();
	}
}

/** This class corresponds to exactly one record in the ACL table in the RIPADMIN database.
 *  
 *  Although some of the column names in the database does not really cover its function,
 *  I didn't rename them to avoid confusion. 
 *  
 * @author agoston
 *
 */
final class AccessRecord {
	/** number of connections */
	int connections;
	/** number of address passes */
	int addrpasses;
	/** number of denials */
	int denials;
	/** number of queries */
	int queries;
	/** number of referrals to an outer source */
	int referrals;
	/** number of returned public objects */
	int publicobjects;
	/** number of returned private objects */
	int privateobjects;
	/** aged number of returned public objects */
	int publicbonus;
	/** aged number of returned private objects */
	int privatebonus;
	/** last touched timestamp */ 
	long timestamp;
}

/** This class corresponds to exactly one record in the ACCESS table in the RIPADMIN database.
 *  
 *  Although some of the column names in the database does not really cover its function,
 *  I didn't rename them to avoid confusion. 
 *  
 * @author agoston
 *
 */
final class ACLRecord {
	/** returned private object limit */
	int maxbonus;
	/** returned public object limit */
	int maxpublic;
	/** number of denials before setting denied flag */
	int maxdenials;
	/** denied flag */
	boolean deny;
	/** allow address passing */
	boolean trustpass;
	/** number of simultaneous connections allowed before printing a refusal message */
	int threshold;
	/** number of simultaneous connections allowed before refusing any new connections */
	int maxconn;
	/** automatic & user comments about the specific ACL entry */
	String comment;
}

class ACLTest {
	Connection dbCon = null;
	
	ACLRecord fetchACL(IPAddress in) {
		String query = "SELECT maxbonus, maxpublic, maxdenials, deny, trustpass, threshold, maxconn, comment FROM ";
		switch (in.address.length) {
			case IPv4Address.LENGTH:
				query += "acl ";
				break;
				
			case IPv6Address.LENGTH:
				query += "acl6 ";
				break;
		}
		query += WhoisTest.getPrefixWhereClause(in);
		ResultSet rs = WhoisTest.executeQuery(dbCon, query);
		
		ACLRecord ret = new ACLRecord();
		try {
			ret.maxbonus = rs.getInt(1);
			ret.maxpublic = rs.getInt(2);
			ret.maxdenials = rs.getInt(3);
			ret.deny = rs.getBoolean(4);
			ret.trustpass = rs.getBoolean(5);
			ret.threshold = rs.getInt(6);
			ret.maxconn = rs.getInt(7);
			ret.comment = rs.getString(8);
			rs.close();
		} catch (SQLException e) {
			throw new RuntimeException("Error while loading record", e);
		}
		
		return ret;
	}
	
	AccessRecord fetchAccess(IPAddress in) {
		String query = "SELECT connections, addrpasses, denials, queries, referrals, publicobjects, privateobjects, publicbonus, privatebonus, timestamp FROM ";
		switch (in.address.length) {
			case IPv4Address.LENGTH:
				query += "access ";
				break;
				
			case IPv6Address.LENGTH:
				query += "access6 ";
				break;
		}
		query += WhoisTest.getPrefixWhereClause(in);
		ResultSet rs = WhoisTest.executeQuery(dbCon, query);
		
		AccessRecord ret = new AccessRecord();
		try {
			ret.connections = rs.getInt(1);
			ret.addrpasses = rs.getInt(2);
			ret.denials = rs.getInt(3);
			ret.queries = rs.getInt(4);
			ret.referrals = rs.getInt(5);
			ret.publicobjects = rs.getInt(6);
			ret.privateobjects = rs.getInt(7);
			ret.publicbonus = rs.getInt(8);
			ret.privatebonus = rs.getInt(9);
			ret.timestamp = rs.getLong(10);
			rs.close();
		} catch (SQLException e) {
			throw new RuntimeException("Error while loading record", e);
		}
		
		return ret;
	}
	
	public void runTest() {
		dbCon = WhoisTest.connectDB();
		
		
		
		WhoisTest.closeDB(dbCon);
	}
}

/** Main class
 * 
 * Also acts as the utility class (for now, as the code is relatively small)
 * 
 * @author agoston
 *
 */

public class WhoisTest {
	public static String ripConfigFileName = "../../conf/rip.config";
	public static final List<String> knownParams = Arrays.asList("-c");
	public static Properties config = new Properties();

	/** Connects to the mysql db specified in rip.conf */
	public static Connection connectDB() {
		Connection con = null;
		
		String ripAdminLine = config.getProperty("RIPADMIN");
		
		if (ripAdminLine == null) {
			throw new RuntimeException("RIPADMIN not found in rip.conf file");
		}
		
		String[] ripAdminDB = ripAdminLine.split(",");
		if (ripAdminDB.length != 5) {
			throw new RuntimeException("RIPADMIN line in rip.conf is invalid");
		}
		
		String url = "jdbc:mysql://"+ripAdminDB[0]+":"+ripAdminDB[1]+"/"+ripAdminDB[4];
		
		try {
			Class.forName("com.mysql.jdbc.Driver");
			con = DriverManager.getConnection(url, ripAdminDB[2], ripAdminDB[3]);
		} catch (Exception e) {
			throw new RuntimeException("Connection failed", e);
		}
		
		return con;
	}
	
	/** No comment :) */
	public static void closeDB(Connection con) {
		try {
			con.close();
		} catch (SQLException e) {
			throw new RuntimeException("Error while closing DB connection", e);
		}
	}
	
	/** Convenience function to avoid handling SQLException for each query */
	public static ResultSet executeQuery(Connection con, String query) {
		ResultSet rs = null;
		
		try {
			rs = con.createStatement().executeQuery(query);
		} catch (SQLException e) {
			throw new RuntimeException("Query execution failed", e);
		}
		return rs;
	}

	/** Convenience function to avoid handling SQLException for each query */
	public static int executeUpdate(Connection con, String query) {
		int ret = -1;
		
		try {
			ret = con.createStatement().executeUpdate(query);
		} catch (SQLException e) {
			throw new RuntimeException("Query execution failed", e);
		}
		return ret;
	}

	public static String getPrefixWhereClause(IPAddress in) {
		String ret = null;
		switch (in.address.length) {
		case IPv4Address.LENGTH:
			ret = "WHERE "+in.getHex(0)+" BETWEEN prefix AND (prefix+(1<<(32-prefix_length))-1) LIMIT 1";
			break;
			
		case IPv6Address.LENGTH:
			ret = String.format("WHERE CASE prefix_length&224 "+
					"WHEN 0 THEN %1$s&(((1<<prefix_length)-1)<<(32-prefix_length)) = prefix1 "+
					"WHEN 32 THEN %1$s = prefix1 AND %2$s&(((1<<(prefix_length-32))-1)<<(64-prefix_length)) = prefix2 "+
					"WHEN 64 THEN %1$s = prefix1 AND %2$s = prefix2 AND %3$s&(((1<<(prefix_length-64))-1)<<(96-prefix_length)) = prefix3 "+
					"WHEN 96 THEN %1$s = prefix1 AND %2$s = prefix2 AND %3$s = prefix3 AND %4$s&(((1<<(prefix_length-96))-1)<<(128-prefix_length)) = prefix4 "+
					"WHEN 128 THEN %1$s = prefix1 AND %2$s = prefix2 AND %3$s = prefix3 AND %4$s = prefix4 "+
					"END LIMIT 1",
					in.getHex(0), in.getHex(1), in.getHex(2), in.getHex(3));
			break;
		}
		
		return ret;
	}

	public static void main(String[] args) {
		for (int i = 0; i < args.length; i++) {
			int pos = knownParams.indexOf(args[i]);
			
			if (pos < 0) {
				System.out.println("whoistest params:");
				System.out.println("    -c <file>    Set config file (def: ../../conf/rip.config)");
				System.out.println("");
				System.exit(1);
			}
			
			if (pos == 0) {
				ripConfigFileName = args[i];
				i++;
			} else if (pos == 1) {
				// ... Whatever ...
			}
		}
		
		try {
			config.load(new FileInputStream(ripConfigFileName));
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
		
		ACLTest aclTest = new ACLTest();
		aclTest.runTest();
	}
}
