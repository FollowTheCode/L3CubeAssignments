
/* ================================================================
 * Title - To find out the duplicate files in a Hard Drive
 *  ---------------------------------------------------------------
 * Author - Follow The Code
 * 	---------------------------------------------------------------
 * Date - 5th June 2014
 * 	---------------------------------------------------------------
 * Brief description: Enlisting all the duplicate files in a Hard Drive 
 * and gives an option to delete them by looking to its path.
 * =================================================================
 * */

import java.io.File;
import java.sql.Date;
import java.util.HashSet;
import java.util.Scanner;
import java.util.Set;

class Scan
{
 /* Storing unique values */
 static Set<String> s = new HashSet<String>();
 /* Storing duplicate files */
 static String dup[]= new String[100];
 /* Storing path of the duplicate files */
 static String path[]= new String[100];
 static String name[]= new String[100];
 /* Not to consider the file which is not repeated */
 static int check[]=new int[10];
 static int k=0;
 static int l=0;
 public static void listContents(File f,String x)
 {
	 //fetch names of all contents (file and directories) of current object (directory)
	 String contents[] = f.list(); 
	 int i,j;
	 for(i =0 ; i< contents.length; i++)
	 {
		 File fi = new File(x+contents[i]);
		 String result=x+contents[i];
		 // Checking for duplicate entries
		 if(s.contains(contents[i]) &&  !fi.isDirectory())
		 {
			 dup[k]= (new String(result));
			 k++;
			 j=0;
			 while(j<l)
			 {
				 if(contents[i].equals(name[j])&&check[j]==1)
				 {
					  	dup[k]= (new String(path[j]));
					  	k++;  
					  	check[j]=0;
				 }
				 j++;
			 }
		 }
		 
		 if(!fi.isDirectory())
		 {
			//Set for storing the unique location of a file
			 if(s.add(contents[i])) 
			 {
				 check[l]=1;
				 name[l]=contents[i];
				 path[l++]=result;
			} 
		 }
		 if(fi.exists())
		 {
			 if(fi.isDirectory())
			 {
				//Recursion for checking the directories for the duplicate files
				 listContents(fi,x+contents[i]+"\\");
			 }
		 }
	 }
}
 
 public static void main(String args[])
 {
	 @SuppressWarnings("resource")
	 Scanner s = new Scanner(System.in);
	 try
	 {
		 System.out.print("Enter the name of the Drive: ");
		 String x = s.next(); 
		 // a File object representing x
		 File f = new File(x);
		 //returns a boolean indicating exists status for a file/directory
		 if(f.exists())
		 {
			//returns a boolean indicating isDirectory status
			 if(f.isDirectory())
			 {
				 System.out.println("Directory/Drive: "+x);
				 listContents(f,x);
				 System.out.println("---------------------------------------------------------");
				 System.out.println("Duplicates Files: ");
				 System.out.println("--------------------------------------------------------------------------------------------------------------------------");
				 for(int i =0;i<k;i++)
				 {
					File fd = new File(dup[i]);
					Date d = new Date(fd.lastModified());
					System.out.printf("%d:  %-20s \tSize (bytes)= %-10d ",i,dup[i],fd.length());
					
					if(fd.canRead())
					{
						System.out.print("\tReadable");
					}
					else
					{
						System.out.print("\tNon Readable");
					}
					if(fd.canWrite())
					{
						System.out.print("\tWritable");
					}
					else
					{
						System.out.print("\tNon Writable");
					}
					System.out.printf("\tLast Modified: %-10s",d.toString());
					System.out.println();
					
					
				 }
				 System.out.println("--------------------------------------------------------------------------------------------------------------------------");
				 System.out.print("Do you want to delete any file from the above list(y/n): ");
				 String ch=s.next();
				 if(ch.equals("y")||ch.equals("Y"))
				 {
					 System.out.print("Select the index number of the file to be deleted: ");
					 int index = s.nextInt();
					 File fi = new File(dup[index]);
					 fi.delete();
					 System.out.println("Deleted");
				 }
				 else
				 {
					 System.out.println("Ignored");
				 }
				 
		  	}
		 	//	f.delete() - to delete a file or an empty directory
		 	//	f.renameTo(f1) - to rename file f as file f1
	  		else
	  		{
		  		System.out.println(x + " is neither a directory nor a file ");
			}
		 }
	 }
	 catch(ArrayIndexOutOfBoundsException ex)
	 {
		 System.out.println("Array out of bound Exception! The selected drive is extremely large, Please select a smaller drive.");
	 }
	 //to handle unseen exceptions
	 catch(Exception ex)
	 {
		 System.out.println(ex);
	 }
 	}//main
}//End of class
