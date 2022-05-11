// package builtinTest;

import System.out.println;
import System.out.print;
import System.in.read;
import Character.toString;

public class MainClass {
    public static void main(String[] args) {
        char inChar;
        System.out.println("Enter a Character:");
        inChar = System.in.read();
        System.out.print("You entered ");
        System.out.println(Character.toString(inChar));
    }
}