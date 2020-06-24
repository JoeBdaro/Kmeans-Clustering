//CS323-34
//Joseph Bdaro

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

class kmeans {
public:
	class pointClass {
	public:
		double Xcoord;
		double Ycoord;
		int label;
		double distance;

		pointClass() {
			this->Xcoord = 0;
			this->Ycoord = 0;
			this->label = 0;
			this->distance = 99999.0;
		}

		pointClass(double x, double y) {
			this->Xcoord = x;
			this->Ycoord = y;
			this->label = 0;
			this->distance = 99999.0;
		}

		pointClass(double x, double y, int label) {
			this->Xcoord = x;
			this->Ycoord = y;
			this->label = label;
			this->distance = 99999.0;
		}
	};


	//members of class
	int numPts;
	int k;
	int numRows;
	int numCols;
	int **displayAry;
	int change;
	pointClass *pointSet;
	pointClass *kcentroidAry;

	kmeans(int k, int numRows, int numCols, int numPts) {
		//initializes main variables
		this->k = k;
		this->numRows = numRows;
		this->numCols = numCols;
		this->numPts = numPts;

		//initializes the pointset class
		pointSet = new pointClass[numPts];
		for (int i = 0; i < numPts; i++) {
			pointClass *initializer = new pointClass();
			pointSet[i] = *initializer;
		}

		//initializes the display array 
		displayAry = new int *[numRows];
		for (int i = 0; i < numRows; i++) {
			displayAry[i] = new int[numCols];

		}
		for (int i = 0; i < numRows; i++) {
			for (int j = 0; j < numCols; j++) {
				displayAry[i][j] = -999;
			}
		}
		
		//inititalizes the kcentroid array
		kcentroidAry = new pointClass[k + 1];
		for (int i = 0; i + 1 < k + 1; i++) {
			pointClass* inititalizer = new pointClass(0, 0, i + 1);
			kcentroidAry[i + 1] = *inititalizer;
		}
		
		change = 0;
	}

	//loadPointSet method (allocated the data from the file to pointset array)
	void loadPointSet(ifstream &inFile) {
		int x, y;
		int index = 0;

		while (inFile >> x >> y) {
			pointSet[index].Xcoord = (double)x;
			pointSet[index].Ycoord = (double)y;
			pointSet[index].label = 0;
			pointSet[index].distance = 99999.00;
			index++;
		}
	}

	void kmeansClustering(kmeans *cluster, ofstream& outFile1) {
		int iteration = 0;
		selectkcentroids(cluster);

		while (change <= 2) {
			int index = 0;
			iteration++;
			PlotDisplayAry(cluster);
			PrettyPrint(cluster, outFile1, iteration);

			while (index < numPts) {
				pointClass pt = cluster->pointSet[index];
				double minDist = pt.distance;
				int minlable = distanceMinLabel(pt, cluster, minDist);

				if (cluster->pointSet[index].label != minlable) {
					cluster->pointSet[index].label = minlable;
					cluster->pointSet[index].distance = distanceFix;
					change++;
				}

				index++;
			}
			if (change > 2) {
				computeCentoids(cluster);
				change = 0;
			}
			else {
				break;
			}
		}

	}

	void selectkcentroids(kmeans *cluster) {
		int kcnt = 0;

		while (kcnt < k) {
			bool repeatYN = true;
			vector<int> temp;
			int index;
			while (repeatYN == true) {
				index = rand() % numPts - 1;
				repeatYN = checkRepeat(index, temp);
			}
			kcnt++;
			kcentroidAry[kcnt].Xcoord = cluster->pointSet[index].Xcoord;
			kcentroidAry[kcnt].Ycoord = cluster->pointSet[index].Ycoord;
			kcentroidAry[kcnt].label = kcnt;
			kcentroidAry[kcnt].distance = 0;

		}
	}
	bool checkRepeat(int index, vector <int> temp) {
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i] == index) {
				cout << "t";
				return true;
			}
		}
		temp.push_back(index);
		return false;	
	}
	void PlotDisplayAry(kmeans* cluster) {
		int iterator = 0;
		while (iterator < numPts) {
			cluster->displayAry[(int)cluster->pointSet[iterator].Xcoord][(int)cluster->pointSet[iterator].Ycoord] = cluster->pointSet[iterator].label;
			iterator++;
		}
	}

	void PrettyPrint(kmeans* cluster, ofstream& outFile, int iteration) {
		outFile << "K: " << cluster->k << ", itteration: " << iteration <<endl;
		for (int i = 0; i < numRows; i++) {
			for (int j = 0; j < numCols; j++) {
				if (displayAry[i][j] == -999) {
					outFile << " ";
				}
				else {
					outFile << displayAry[i][j];
				}
			}
			outFile << endl;
		}
		

	}
	double distanceFix;
	int distanceMinLabel(pointClass pt, kmeans *cluster, double minDist){
		minDist = 9999.00;
		int minLable = 0;
		int lable = 1;

		while (lable <= k) {
			pointClass whichCentroid = cluster->kcentroidAry[lable];
			double dist = computeDist(pt, whichCentroid);

			if (dist < minDist) {
				minLable = lable;
				minDist = dist;
			}
			lable++;
		}
		distanceFix = minDist;
		return minLable;
	}

	double computeDist(pointClass pt, pointClass whichCentroid) {
		double result = (sqrt(pow(pt.Xcoord - whichCentroid.Xcoord, 2) + (pow(pt.Ycoord - whichCentroid.Ycoord, 2))));
		return result;
	}

	void computeCentoids(kmeans* cluster) {
		double* sumX = new double[k + 1];
		double* sumY = new double[k + 1];
		int* totalPt = new int[k + 1];

		for (int i = 0; i + 1 < k + 1; i++) {
			sumX[i] = 0.0;
			sumY[i] = 0.0;
			totalPt[i] = 0;
		}

		int index = 0;
		
		while (index < numPts) {
			int lable = cluster->pointSet[index].label;
			sumX[lable] += cluster->pointSet[index].Xcoord;
			sumY[lable] += cluster->pointSet[index].Ycoord;
			totalPt[lable] ++;
			index++;
		}
		int lable = 1;

		while (lable <= k) {
			if (totalPt[lable] > 0) {
				cluster->kcentroidAry[lable].Xcoord = (sumX[lable] / totalPt[lable]);
				cluster->kcentroidAry[lable].Ycoord = (sumY[lable] / totalPt[lable]);
			}
			lable++;

		}



	}

	void printResult(kmeans* cluster, ofstream& outFile2) {
		outFile2 << cluster->numRows << " " << cluster->numCols <<endl;
		outFile2 << numPts << endl;
		outFile2 << "k: " << cluster->k << endl;

		for (int i = 0; i < numPts; i++) {
			outFile2 << cluster->pointSet[i].Xcoord << " " << cluster->pointSet[i].Ycoord << " " << cluster->pointSet[i].label << endl;

		}

	}


};


int main(int argc, char* argv[])
{
	ifstream inFile;
	ofstream outFile1, outFile2;
	int k = strtol(argv[2], NULL, 10);
	inFile.open(argv[1]);
	outFile1.open(argv[3]);
	outFile2.open(argv[4]);
	int numRows;
	int numCols;
	int numPts;

	inFile >> numRows;
	inFile >> numCols;
	inFile >> numPts;

	kmeans *newCluster = new kmeans(k, numRows, numCols, numPts);
	newCluster->loadPointSet(inFile);
	newCluster->kmeansClustering(newCluster, outFile1);
	newCluster->printResult(newCluster, outFile2);

	inFile.close();
	outFile1.close();
	outFile2.close();




}

