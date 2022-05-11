public class test1class { 

    public static void main(String[] args) {

        //int q = 1;
        
        //int [] i = {0, 100};
        //System.out.println("print");

        //check right and left base type
        //int[] myarray = new String [10]; //Throws Type Mismatch Error

        int[] myarray = new int[10];

        myarray[10] = 1;

        // myarray[10] = '1';      //Throws Type Mismatch Error 

        //myarray[11] = 1;  //THROWS ARRAY SIZE ERROR

        int b;

        b = 5;
        int c  = 0;
        long g = 15l;
        double e;
        //b = 1l + b;   //Thwors type Mismatch Error

        //b = a + b; // a is undeclared error

        //b = 1 + 2 + (b + a ); // a is undeclared error

        b = 1 + 2 + (b + 1);
       
        if ( b == 1){
            
            //b =1l; //type Mismatch error
            b = 1;
        }else if( 1 == 1 ){

            //g = '1'; //Type Mismatch error
            g = 2l;
        }else {
            //i = 1;    //Throws Undeclared Error
            b =5;
        }
        
        //char c; // redefining c should throw an error...        
        /*TODO:: does not support this atm
        /*public static void myMethod() {
            System.out.println("I just got executed!");
        }



        /*this throws error atm*/ //boolean b = false; // in J0, local variables may not be assigned on the same line as they are declared.
        
    
        //String s = "hello";

        //we need to make our program accept these as well
        // b = true; // Breaks on bools
        // i = 100;
        // l = 100000000l;
        // f = 3.141592f;
        // d = 3.71828;
        // c = 'n';
        // c = '\n';
        // c = '\'';
        // c = '\\';
        // c = '\a';
        // c = '\f';
        // c = '"';
        // s = "world";
    }
} //single line comment
