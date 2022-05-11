import System.out.println;

public class HelloWorld {
    public static String hwGenerator(){
        return "Hello\t\"\144\\ \rWor\tld.\n";
    }

    public static void main(String[] args){
        String message = hwGenerator();
        System.out.println(message);
    }
}