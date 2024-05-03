package histogram;

public class TesterUtils {
	public static void printErrorNum(int num){
		System.err.println("ERROR " + num);
	}
	
	public static void printErrorNum(int num, String expected, String result){
		System.err.println("ERROR " + num + ": expected [" + expected + "] , got [" + result + "]" );
	}
	
	
	public static void checkDoubleValueAndPrintError(double expected, double result, double eps, int errorNum){
		if (Math.abs(expected-result) > eps){
			printErrorNum(errorNum, ""+expected, ""+result);
		}
	}
	
}
