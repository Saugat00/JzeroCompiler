//package abc; 

// import System.out.println;
// import Integer.toString;

public class HelloWorld {
    public static void main(String[] args) {
        int i = 1 + 2;
        boolean bool; // in J0, local variables may not be assigned on the same line as they are declared.
        float flt;
        // int i = -1; // redeclaration error
        char c;
        // bool = true;
        flt = 3.141592f;
        i = 100;
        c = '\n'; //this accidentally stored as value of \ instead of \n
        countTo4();
        i = -(i+1);
        // i = x;
        // x = i;
        // printGreeting(); // throws missing parameters error
        // printGreeting(1, 2.0f); // throws insufficient number of paramters error
        // printGreeting(1,2,3.0); // throws type mismatch in parameter 2 error
        double d = printGreeting(10, flt, 3.0); // throws no errors
    
        // i = listCarMakers(listCarMakers(1,0), i);
        i = listCarMakers(i, listCarMakers(1,0));
        //System.out.println("Hello, World!"); 
        // int i = 1; // redeclaration error
    }
    
    //j0 only supports public static methods
    public static double printGreeting(int a, float b, double c) {
        int time;
        time = a; 
        
        if (time < 10) {
            // System.out.println("Good morning.\n");
            time = 0;
        } else if (time < 20) {
            // System.out.println("Good \t day.\n");
            time = 4;
        } else {
            // System.out.println("Good evening.\n");
            time = 1;
        }
    }
    
    public static int listCarMakers(int c, int d){
        String car;
        car = "Ford F150";
        // System.out.println(car);
        return 1;
    }
    
    // public static void listCarMakers(int c){ // throws a method redeclaration error
    //     int error;
    // }
    
    public static void countTo4() {
        int i;
        i = 0;
        while (i < 5) {
            // System.out.println(Integer.toString(i));
            i++;
        }
    }

}
    