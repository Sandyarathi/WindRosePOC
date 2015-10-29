/*
 * Extraction and Parser java code to extract 
 * the netcdf raw data to csv files.And also to create the File List for Iteration.
 * Fields extracted : Station Id, Observation datetime, 
 * Latitude, Longitude, Wind Direction, Wind Speed
 * Data extracted : Year 2001-2014, July 01-Sep 30, 2300hrs
 */
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileFilter;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;

import ucar.nc2.NetcdfFile;


public class extractdata {
	public static void main(String[] args) throws IOException {	

		System.out.println("Extracting Data");
		NetcdfFile nf = null;
		//NetcdfFile nfFileList = null;
		
		File rawdataDir = new File("/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/");
		File outdir = new File("/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/2001-2014extractedData/");
		
		if (rawdataDir.exists()) {
			
			BufferedWriter brFileList = new BufferedWriter(new FileWriter(rawdataDir + "/newFile.txt"));
			StringBuilder sbFileList = new StringBuilder();
			
			List<String> files = list(rawdataDir);
	        for (String file : files) {
	    		char[][] tmp;
	    		nf = NetcdfFile.open(file);
				String[] stationId = null;
				tmp = (char[][]) nf.findVariable("stationId").read().copyToNDJavaArray();
				stationId = new String[tmp.length];
				for (int n = 0; n < tmp.length; n++)
					stationId[n] = String.valueOf(tmp[n]).trim();
				
				
				float[] lat = (float[]) nf.findVariable("latitude").read().copyTo1DJavaArray();
				float[] lon = (float[]) nf.findVariable("longitude").read().copyTo1DJavaArray();
				double[] obstime = (double[]) nf.findVariable("observationTime").read()
						.copyTo1DJavaArray();
	    		float[] windspd = (float[]) nf.findVariable("windSpeed").read().copyTo1DJavaArray();
	    		float[] winddir = (float[]) nf.findVariable("windDir").read().copyTo1DJavaArray();
	    		Path filepath = Paths.get(file);
	    		String filename = filepath.getFileName().toString();
	    		String filepath1 = filepath.getParent().toString();
	    		
	    		String newFileName = filepath1 + "mesonet-" + filename + ".csv";
	    		
	    		System.out.println("newFileName : " + newFileName);
	    		
	    		BufferedWriter br = new BufferedWriter(new FileWriter(newFileName));
	    		StringBuilder sb = new StringBuilder();
	    		for(int i=0; i<windspd.length; i++)
	    		{
	    			if( ( windspd[i] >= 0 && windspd[i] <=130) && (winddir[i]>=0 && winddir[i]<=360) ) {
	    				sb.append(stationId[i]);
	    				sb.append(",");
	    				Date obsDate = new Date((long) obstime[i] * 1000);
	    				sb.append(obsDate);
	    				sb.append(",");
	    				sb.append(lat[i]);
	    				sb.append(",");
	    				sb.append(lon[i]);
	    				sb.append(",");
	    				sb.append(String.format("%.2f", windspd[i]));
	    				sb.append(",");
	    				sb.append(String.format("%.2f", winddir[i]));
	    				sb.append("\n");
	    			}
	    		}
	    		
	            sbFileList.append(newFileName + "\n");
	            
	    		br.write(sb.toString());
	    		br.close();	
	        }
	        
	        brFileList.write(sbFileList.toString());
	        brFileList.close();	
		}
		else{
			System.out.println("Some problem with file!");
		}
	}
	
	
	protected static List<String> list(File parent) {
		return listFiles(parent, parent);
	}

	protected static List<String> listFiles(File parent, File folder) {
		List<String> lstFiles = new ArrayList<String>(25);
		if (folder.isDirectory()) {			
			File[] files = folder.listFiles();
			if (files != null) {
				for (File file : files) {
					if (file.isDirectory()) {
						lstFiles.addAll(listFiles(parent, file));
					} 
					else {
						if(!file.getName().startsWith(".") && !file.getName().endsWith(".gz")  && !file.getName().endsWith(".txt")  && !file.getName().endsWith(".csv"))
						{
							String path = file.getAbsolutePath();
							lstFiles.add(path);
						}
					}
				}
			}
		}
	
		return lstFiles;
	}

}
