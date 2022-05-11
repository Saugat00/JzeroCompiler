import System.out.println;
import Integer.toString;
import string.length; // will have to use string in lower case because String with upper case is a reserved word... unfortunate
import string.charAt;

public class oopSpoofTest {
    public static void main (String[] args) {
        String s = "Hello World";
        int len;
        len = s.length();
        System.out.println(Integer.toString(len));
        char c;
        c = s.charAt(2); // running this line with the -d flag causes segfault, bot otherwise passes test... investigate later
    }
}