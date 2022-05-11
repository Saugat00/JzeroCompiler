public class codeGen
{
    public static void write(int j){}

    public static void main(String[] args)
    {
        long i;
        i = 5;
        // i = (1 + i) * i;
        i = i * i + 1;
        // i = 1 + (i * i); // icode gen breaks on this even though it should be an equivalent expression
        write(i);
    }
}