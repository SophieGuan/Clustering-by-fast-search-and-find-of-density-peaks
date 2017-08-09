#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>

using namespace std;

#define M 1000
#define largeno 99999
#define epsilon 1.8 //distance parameter
#define clusterNo 2 //# of clusters

int N=240;//number of input data

//define the structure of input data,can vary according to different datasets
struct rawdata
{
	int id;
	double x;
	double y;
};

//used to store id of the nearest neighbor with higher density and distance between them
struct densityNeighbor
{
	int nid;
	double distance;
};

//a function to return get the cluster it belongs to with cluster center determined
int neighborCluster(int i,int cluster[],struct densityNeighbor neighborDistance[])
{
    //a point should be put in the same cluster with its nearest neighbor which has a higher density
	if(cluster[neighborDistance[i].nid]!=-1)
		return cluster[neighborDistance[i].nid];
	else
		return neighborCluster(neighborDistance[i].nid,cluster,neighborDistance);
}

//store the data into the structure array defined above
void initialize(struct rawdata data[],int N)
{
	string value;
    //file name is "flame.csv"
	ifstream file ( "flame.csv" );
	int a,i=0;
	double b;
	while(file.good() & (i<N))
	{
        //the first value is a real number, indicating x coordinate
		getline(file,value,',');
		b=atof(value.c_str());
		data[i].x=b;

        //the second value is a real number, indicating y coordinate
		getline(file,value,',');
		b=atof(value.c_str());
		data[i].y=b;

        //indicating the cluster it belongs to 
		getline(file,value,'\n');
		a=atoi(value.c_str());
		data[i].id=a;
		i++;		
	}
}

//calculate the distance between any two points
void calDistance(double distance[][M], struct rawdata data[],int N)
{
	int i,j;
	for(i=0;i<N;i++)
	{
		for(int j=i;j<N;j++)
		{
            //use Euclidean distance to denote the distance
            //just need to calcultate half of the matrix as it is symmetric
			distance[i][j]=sqrt((data[i].x-data[j].x)*(data[i].x-data[j].x)+(data[i].y-data[j].y)*(data[i].y-data[j].y));
		}
	}
	for(i=0;i<N;i++)
	{
		for(j=i;j>=0;j--)
		{
            //get result of another half matrix
			distance[i][j]=distance[j][i];
		}
	}
	for(i=0;i<N;i++)
        //set diagonal number to a large integer 
		distance[i][i]=largeno;

	//display distance matrix
	/*for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			cout<<setfill(' ')<<setw(6)<<setprecision(4)<<distance[i][j]<<" ";
		}
		cout<<endl;
	}*/	
}

//store number of neighbors for each point into an array neighborCount[]
void countNeighbor(int neighborCount[],double distance[][M],int N)
{
	
	int i,j;
    for(i=0;i<N;i++)
    	neighborCount[i]=0;
    for(i=0;i<N;i++)
    {
    	for(j=0;j<N;j++)
    	{
            //traverse all the point, if distance less than epsilon, count+1
    		if (distance[i][j]<epsilon)
    			neighborCount[i]++;
    	}
    }
    //output the neighborCount array
    /*for(i=0;i<N;i++)
    	cout<<neighborCount[i]<<endl;*/
}

//for each point, find a point with higher density and nearest distance
//store the point ID and distance in 1-D structure array neighborDistance[]
void getMinDistance(int neighborCount[],double distance[][M],struct densityNeighbor neighborDistance[],int N)
{
	int i,j;
    //output the neighbor count and distance into a text file
    //the file is used to draw decision graph later
    ofstream decisionGraph("decisionGraph.txt");
	for(i=0;i<N;i++)
    {
		int a=-1;
    	double temp=largeno;
    	for(j=0;j<N;j++)
    	{
            //traverse all the points and find the nearest densier neighbor
    		if(neighborCount[j]>neighborCount[i]&distance[i][j]<temp)
    		{
    			a=j;
    			temp=distance[i][j];
    		}

    	}
        //a=-1 indicates that the point is the one with largest density, then we should find the point with the largest distance 
    	if(a==-1)
    	{
    		temp=0;
    		for(j=0;j<N;j++)
    		{
                //the point cannot be itself
    			if(distance[i][j]>temp & distance[i][j]<largeno)
    			{
    				a=j;
    				temp=distance[i][j];
    			}

    		}
            //store the result in array
    		neighborDistance[i].nid=a;
    		neighborDistance[i].distance=temp;
    	}
    	else
    	{
    		neighborDistance[i].nid=a;
    		neighborDistance[i].distance=temp;
    	}
    }
    //cout<<"Calculation results:"<<endl;
    //cout<<"id    # of points    distance"<<endl;
    for(i=0;i<j;i++)
    {
        //can output the result to screen
        //cout<<setw(3)<<setfill(' ')<<i+1<<"        "<<neighborCount[i]<<"         "<<neighborDistance[i].distance<<endl;
        //store result in the text file with name"DecisionGraph.txt"
        decisionGraph<<i+1<<" "<<neighborCount[i]<<" "<<neighborDistance[i].distance<<endl;
    }   
    cout<<endl;
}

//Given decision graph and numbers of clusters, this function is used to find the cluster center
void findClusterCenter(int neighborCount[],struct densityNeighbor neighborDistance[],int cluster[])
{
	int i,j;
	double multiply[N];

    for(i=0;i<N;i++)
    {
        //calculate the multiplication of neighborno and distance
    	multiply[i]=neighborCount[i]*neighborDistance[i].distance;
    	cluster[i]=-1;
    }
    cout<<"Cluster center:"<<endl;
    for(i=1;i<=clusterNo;i++)
    {
        //find the largest number in multiply array and set this point as cluster center
    	double temp=0;
    	int c=-1;
    	for(j=0;j<N;j++)
    	{
    		if(multiply[j]>temp)
    		{
    			temp=multiply[j];
    			c=j;
    		}	
    	}
    	cluster[c]=i;
    	cout<<"cluster center "<<i<<":point "<<c+1<<endl;
        //reset the value to zero so that it cannot be found a second time
    	multiply[c]=0;
    }
    cout<<endl;
}

//given cluster center, this function is used to put all other points into corresponding cluster
void clusterResult(struct densityNeighbor neighborDistance[],int cluster[])
{
	int i;
    //count the number of points for each cluster
    int countNo[clusterNo];
    //store the cluster result in a text file named "flameResult.txt"
    ofstream output( "flameResult.txt" );
    //initialize
    for(i=0;i<clusterNo;i++)
        countNo[i]=0;

    //find its density neighbor for each point and decide the cluster it belongs to 
	for(i=0;i<N;i++)
    {
    	if(cluster[i]==-1)
    	{
    		cluster[i]=neighborCluster(i,cluster,neighborDistance);
    	}		

    }	

    cout<<"Cluster result:"<<endl;
    for(i=0;i<N;i++)
    {

    	//cout<<"point "<<setw(3)<<setfill(' ')<<i+1<<" belongs to cluster "<<cluster[i]<<endl;
        countNo[cluster[i]-1]++;
        //output the cluster result to file
        output<<i+1<<" "<<cluster[i]<<endl;
    }

    for(i=0;i<clusterNo;i++)
        {
            //output no of points for each cluster
            cout<<"Cluster "<<i+1<<" has "<<countNo[i]<<" points."<<endl;
        }
}

int main()
{
	struct rawdata data[M];
	int i=0,j=0,a;
	initialize(data,N);	
    //can output data to screen
	/*for(i=0;i<N;i++)
	{
		cout<<"The "<<i+1<<"th element:"<<data[i].id<<" "<<data[i].x<<" "<<data[i].y<<endl;
	}*/

	double distance[N][M];
	calDistance(distance,data,N);
	
//count neighbors for each point
	int neighborCount[N];
    countNeighbor(neighborCount,distance,N);
    

//get distance between each point and the point with a higher density 
    struct densityNeighbor neighborDistance[N];
 	getMinDistance(neighborCount,distance,neighborDistance,N);
    
    int cluster[N];
    findClusterCenter(neighborCount,neighborDistance,cluster);
    
    clusterResult(neighborDistance, cluster);

//calculate the accuracy of cluster result
    int accurate=0;
    
    for(i=0;i<N;i++)
    {
        if(cluster[i]==data[i].id)
            accurate++;
    }

//output the accuracy number in percentage
    cout<<"Accuracy: "<<setprecision(4)<<double(100*max(accurate,N-accurate))/N<<'%'<<endl;
   
    return 0;
}