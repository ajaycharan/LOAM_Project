#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <Eigen\Core>
#include <random>
#include <fstream>
#include <iterator>
#include <map>
#include "LinearAlgebraFns.h"

using namespace Eigen;

class LoamPt
{
public:
	//std::vector<double> xyz;
	Vector3d xyz;
	int sweepID;
	int sliceID;
	std::vector<int> nearPt1, nearPt2, nearPt3; // these will be len 2 vectors with pt = {sliceIndex, pointIndex}
	int filled;
	double timeStamp;

	LoamPt();
	~LoamPt();
	LoamPt(const std::vector<double> &xyzInput); // no time given
	LoamPt(const std::vector<double> &xyzInput, const double time);
	LoamPt(const double x, const double y, const double z, const double time);
	LoamPt(const LoamPt &otherPt);
	LoamPt &operator = (const LoamPt &otherPt);
	std::vector<double> Minus(const LoamPt &otherPt);
	std::vector<double> Minus(const std::vector<double> &otherPt);

	//static inline int Size(const std::vector<double> &xyz);

	bool SetXYZ(const std::vector<double> &newXYZ);

	inline const double GetX();
	inline const double GetY();
	inline const double GetZ();
	inline const double GetT();

	bool Distance(double &dist, LoamPt &otherPt);
	Vector3d Transform(Matrix4d &xformMatrix4x4);
	//Vector3d Transform(const std::vector<double[4]> &xformMatrix4x4); // xformMatrix is stored as stacked row-vectors
	void TransformSelf(Matrix4d &xformMatrix4x4); // xformMatrix is stored as stacked row-vectors
};

LoamPt::LoamPt()
{
	filled = 0;
}

LoamPt::~LoamPt()
{
}

LoamPt::LoamPt(const LoamPt &otherPt) // copy constructor
{
	if (otherPt.filled == 1)
	{
		xyz = otherPt.xyz;
		timeStamp = otherPt.timeStamp;
		filled = 1;
	}
	else
	{
		std::cout << "You are either trying to self assign, or the incoming pt has not been initialized/filled" << std::endl;
	}
}

LoamPt &LoamPt::operator = (const LoamPt &otherPt) // copy operator
{
	if (this != &otherPt && otherPt.filled == 1) // if the incoming instance is a different pt and has values
	{
		xyz = otherPt.xyz;
		timeStamp = otherPt.timeStamp;
		filled = 1;
	}
	else
	{
		std::cout << "You are either trying to self assign, or the incoming pt has not been initialized/filled" << std::endl;
	}
	return *this;
}

std::vector<double> LoamPt::Minus(const LoamPt &otherPt)
{
	std::vector<double> newVec = { xyz[0] - otherPt.xyz[0], xyz[1] - otherPt.xyz[1], xyz[2] - otherPt.xyz[2] };
	return newVec;
}

std::vector<double> LoamPt::Minus(const std::vector<double> &otherPt)
{
	std::vector<double> newVec = { xyz[0] - otherPt[0], xyz[1] - otherPt[1], xyz[2] - otherPt[2] };
	return newVec;
}

LoamPt::LoamPt(const std::vector<double> &xyzInput)
{
	if (SetXYZ(xyzInput) == true)
	{
		timeStamp = NULL;
		filled = 1;
		std::cout << "Warning, the incoming datapoints have no timestamps" << std::endl;
	}
}

LoamPt::LoamPt(const std::vector<double> &xyzInput, double time) // vector input constructor
{
	std::cout << "In here" << std::endl;
	if (SetXYZ(xyzInput) == true)
	{
		timeStamp = time;
		filled = 1;
	}
}

LoamPt::LoamPt(double x, double y, double z, double time) // individual value constructor
{
	xyz = { x,y,z };
	timeStamp = time;
	filled = 1;
}

bool LoamPt::Distance(double &dist, LoamPt &otherPt)
{
	if ((filled == 1) && (otherPt.filled == 1) && (this != &otherPt)) // if both points are filled
	{
		// calculate pt2pt distance, return success
		dist = sqrt(pow((GetX() - otherPt.GetX()), 2) + pow((GetY() - otherPt.GetY()), 2) + pow((GetZ() - otherPt.GetZ()), 2));
		return true;
	}
	else
	{
		// return failure
		return false;
	}
}

bool LoamPt::SetXYZ(const std::vector<double> &newXYZ)
{
	//int size = Size(newXYZ);
	//std::cout << size << std::endl;
	if (newXYZ.size() == 3)
	{
		// point is set to new value and declared filled
		xyz = { newXYZ[0], newXYZ[1], newXYZ[2] };
		filled = 1;
		return true;
	}
	else
	{
		// point stays the same, filled declaration remains what it was before fill attempt
		std::cout << "The incoming xyz point has " << newXYZ.size() << " values rather than 3" << std::endl;
		return false;
	}
}

inline const double LoamPt::GetX()
{
	if (filled == 1) return xyz[0];
}

inline const double LoamPt::GetY()
{
	if (filled == 1) return xyz[1];
}

inline const double LoamPt::GetZ()
{
	if (filled == 1) return xyz[2];
}

inline const double LoamPt::GetT()
{
	if (filled == 1) return timeStamp;
}

Vector3d LoamPt::Transform(Matrix4d &xformMatrix4x4)
{
	Vector4d augVec = { xyz[0], xyz[1], xyz[2], 1 }, newVec;
	newVec = xformMatrix4x4*augVec;
	return { newVec[0], newVec[1], newVec[2] };
}

//std::vector<double> LoamPt::Transform(const std::vector<double[4]> &xformMatrix4x4)
//{
//	// assumes the 4x4 matrix is filled/correct
//	std::vector<double> newXYZ;
//	for (int i = 0; i < 3; i++)
//	{
//		newXYZ.push_back(xformMatrix4x4[i][0] * GetX() + xformMatrix4x4[i][1] * GetY() + xformMatrix4x4[i][2] * GetZ() + xformMatrix4x4[i][3]);
//	}
//	return newXYZ;
//}

void LoamPt::TransformSelf(Matrix4d &xformMatrix4x4)
{
	// assumes the 4x4 matrix is filled/correct
	Vector4d augVec = { xyz[0], xyz[1], xyz[2], 1 };
	if (filled == 1)
	{
		augVec = xformMatrix4x4*augVec;
		xyz = { augVec[0], augVec[1], augVec[2] };
	}
	else
	{
		std::cout << "Tried transforming an unfilled point" << std::endl;
	}
}

class SliceVector
{
public:
	std::vector<LoamPt> pts;   // vector containing instances of LoamPt class
	std::vector<int> edgePts;  // vector holding the indices of points chosen to lie on edges in this slice
	std::vector<int> planePts; // vector holding the indices of points chosen to lie in planes in this slice

	int vectorIdx, sweepIdx;
	double timeStamp;

	SliceVector();
	~SliceVector();
	SliceVector(std::vector<std::vector<double>> &inputSlice);
	SliceVector(int sweepNumber, int sliceNumber, std::vector<std::vector<double>> &inputSlice);
	void AddPts(std::vector<std::vector<double>> &inputPts);
	void FindEdges();
	void FindPlanes();
	void FindNearest();
};

SliceVector::SliceVector()
{

}

SliceVector::~SliceVector()
{

}

SliceVector::SliceVector(std::vector<std::vector<double>> &inputSlice)
{
	AddPts(inputSlice);
}

SliceVector::SliceVector(int sweepNumber, int sliceNumber, std::vector<std::vector<double>> &inputSlice)
{
	vectorIdx = sliceNumber;
	sweepIdx = sweepNumber;
	AddPts(inputSlice);
}

void SliceVector::AddPts(std::vector<std::vector<double>> &inputPts)
{
	// expected pt format is {x,y,z,time}

	for (auto &elem : inputPts[0])
	{
		std::cout << elem << std::endl;
	}

	//std::cout << inputPts[0][0] << std::endl;

	//std::vector<double> pt = inputPts[0];
	//int size = sizeof(pt) / sizeof(pt[0]);

	std::cout << "Input Pt size = " << inputPts[0].size() << std::endl;

	if (inputPts[0].size() == 4)
	{
		for (auto &line : inputPts)
		{
			pts.push_back(LoamPt({ line[0], line[1], line[2] }, line[3]));
		}
	}
	else if (inputPts[0].size() == 3)
	{
		for (auto &line : inputPts)
		{
			pts.push_back(LoamPt({ line[0],line[1],line[2] }));
		}
	}
}

void SliceVector::FindEdges()
{
	std::vector<double> curvatureValues;
	double curvature;
	for (auto &elem : pts)
	{
		// run kernal;
		// curvature = .......
		curvatureValues.push_back(curvature);
	}
}

class Sweep
{
public:
	std::vector<SliceVector> slices;
	std::vector<std::vector<LoamPt>> ptCloud;			// Cloud of LoamPts where ptCloud[i] = slice of LoamPts, and ptCloud[i][j] = individual LoamPt
	std::map<int, std::vector<int>> edgePts, planePts;	// 2D Vector containing the slice/pt indices of pts declared as edge/plane points
	std::vector<double> timeStamps;						// Vector of time values, where timeStamps[i] is the timeStamp corresponding to slice i of the ptCloud
	double tStart, tEnd;
	std::vector<double> transform;
	int kernalSize = 11, regionPerSlice = 4, edgePerRegion = 2, planePerRegion = 4, edgeFindThreshold = 3;
	Sweep();
	~Sweep();
	void AddSlice(int sweepNumber, int sliceNumber, std::vector<std::vector<double>> &inputSlice);
	void FindAllEdges();
	void FindEdges(int sliceIndex);
	void SortCurvatures(int sliceIdx, std::vector<std::vector<double>> &curveVec, int startIdx, int endIdx);
	bool FindBestEdgePt(int sliceIdx, std::vector<std::vector<double>> &curveVec);
	bool EvaluateEdge(int sliceIdx, std::vector<double> &potentialPt);
	bool FindBestPlanePt(int sliceIdx, std::vector<std::vector<double>> &curveVec);
	bool EvaluatePlane(int sliceIdx, std::vector<double> &potentialPt);
};

Sweep::Sweep()
{

}

Sweep::~Sweep()
{

}

void Sweep::AddSlice(int sweepNumber, int sliceNumber, std::vector<std::vector<double>> &inputSlice)
{
	slices.push_back(SliceVector(sweepNumber, sliceNumber, inputSlice));
	std::vector<LoamPt> slice;
	for (auto &xyzPt : inputSlice)
	{
		slice.push_back(LoamPt(xyzPt));
	}
	ptCloud.push_back(slice);
}

void Sweep::FindEdges(int sliceIdx)
{
	auto &slice = ptCloud[sliceIdx];
	std::vector<std::vector<double>> curvatures; // {curvatureValue, ptIndex}
	int numPts = slice.size();
	curvatures.resize(numPts);
	int firstPt = kernalSize / 2;
	int lastPt = numPts - kernalSize / 2;
	int ptPerRegion = (lastPt - firstPt) / 4;
	Vector3d distVec = { 0,0,0 };

	distVec = kernalSize*slice[firstPt].xyz;

	// find distance vector for initial point
	for (int i = -kernalSize / 2; i < kernalSize / 2; i++)
	{
		distVec -= slice[firstPt + i].xyz;
	}

	curvatures[firstPt] = { distVec.norm() / slice[firstPt].xyz.norm(), (double)firstPt };

	// calculate curvature values for all points after the first point
	for (int i = firstPt + 1; i < lastPt; i++)
	{
		distVec = distVec - kernalSize*(slice[i-1].xyz - slice[i].xyz) + slice[i + kernalSize / 2].xyz - slice[i - kernalSize / 2].xyz;
		curvatures[i] = { distVec.norm() / slice[i].xyz.norm(), (double)i };
	}

	// find and record edges/plains via curvature sorting
	for (int reg = 0; reg < regionPerSlice; reg++)
	{
		SortCurvatures(sliceIdx, curvatures, firstPt + ptPerRegion*reg, firstPt + ptPerRegion*(reg + 1));
	}
}

void Sweep::SortCurvatures(int sliceIdx, std::vector<std::vector<double>> &curveVec, int startIdx, int endIdx)
{
	std::vector<std::vector<double>> curvatures;
	curvatures.resize(endIdx - startIdx);
	
	// get subset of curvature values corresponding to region between startIdx, endIdx
	for (int i = 0; i < endIdx-startIdx; i++)
	{
		curvatures[i] = curveVec[i];
	}

	// sort the curvature vector
	MergeSort(curvatures);

	int edges = 0, planes = 0;

	int planeTurn = false;

	while (curvatures.size() > 0 && (edges < edgePerRegion || planes < planePerRegion))
	{
		// try to find edge point
		if (planeTurn == false && edges < edgePerRegion)
		{
			if (FindBestEdgePt(sliceIdx, curvatures) == true)
			{
				edges++;
				planeTurn = true;
			}
		}
		// try to find plane point
		else if (planes < planePerRegion)
		{
			if (FindBestPlanePt(sliceIdx, curvatures) == true)
			{
				planes++;
				planeTurn = false;
			}
		}
	}
}

bool Sweep::FindBestEdgePt(int sliceIdx, std::vector<std::vector<double>> &curveVec)
{
	// best edges have highest curvature values
	std::vector<double> pt;
	while (curveVec.size() > 0)
	{
		pt = curveVec[curveVec.size()];
		curveVec.erase(curveVec.end());
		if (EvaluateEdge(sliceIdx, pt) == true) // valid edge point
		{
			// save edgePt's {sliceIdx, ptIdx}
			edgePts[sliceIdx].push_back((int)pt[1]);
			return true;
		}
	}
	return false;
}

bool Sweep::FindBestPlanePt(int sliceIdx, std::vector<std::vector<double>> &curveVec)
{
	// best planes have low curvature values
	std::vector<double> pt;
	while (curveVec.size() != 0)
	{
		pt = curveVec[0];
		curveVec.erase(curveVec.begin());
		if (EvaluatePlane(sliceIdx, pt) == true) // valid plane point
		{
			// save planePt's {sliceIdx, ptIdx}
			planePts[sliceIdx].push_back((int)pt[1]);
			return true;
		}
	}
	return false;
}

bool Sweep::EvaluateEdge(int sliceIdx, std::vector<double> &potentialPt) // Checks to make sure that no nearby points are drastically closer to the sensor
{
	double ptDist = ptCloud[sliceIdx][potentialPt[1]].xyz.norm();
	for (int i = -2; i < 2; i++)
	{
		if (ptCloud[sliceIdx][potentialPt[1]+i].xyz.norm() - ptDist > edgeFindThreshold) // nearby point is x (meters) closer to the sensor
		{
			return false;
		}
	}
	// no nearby pts indicate occlusion, pt is valid!
	return true;
}

bool Sweep::EvaluatePlane(int sliceIdx, std::vector<double> &potentialPt) // Checks to make sure that no nearby points are drastically closer to the sensor
{
	Vector3d distVec;
	Vector3d Xi = ptCloud[sliceIdx][potentialPt[1]].xyz;
	distVec = kernalSize*Xi;
	for (int i = -kernalSize / 2; i < kernalSize / 2; i++)
	{
		distVec -= ptCloud[sliceIdx][potentialPt[1] + i].xyz;
	}
	
	if (distVec.dot(Xi) / (distVec.norm()*Xi.norm()) >= 0.5) // a*b = |a||b|cos(theta) ---> dot(a,b)/(|a||b|) = cos(theta)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int main(void)
{
	//char buff[256];
	//std::ifstream input("testfile_cube.bin", std::ios::binary);

	//while (input.read(buff, sizeof(buff)))
	//{
	//	std::cout << buff << std::endl;
	//}

	//std::vector<char> buffer((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));

	LoamPt myNewPt = LoamPt();
	std::vector<double> pt1 = { 1,1,1 }, pt2 = { 0, 1, 0 }, pt3, pt4;

	LoamPt filledPt = LoamPt({ 1,1,1 }, 20);
	myNewPt.SetXYZ(pt2);

	pt3 = filledPt.Minus(myNewPt);
	pt4 = filledPt.Minus(pt2);

	std::vector<std::vector<double>> newSlicePoints;

	for (double i = 0.0; i < 10.0; i++)
	{
		newSlicePoints.push_back({i,i,i});
	}

	std::cout << "Size of this point = " << pt1.size() << std::endl;

	SliceVector ConstructedSlice(newSlicePoints), AddedSlice;

	AddedSlice.AddPts(newSlicePoints);

	AddedSlice.AddPts(newSlicePoints);

	Sweep NewSweep;

	for (int i = 0; i < 5; i++)
	{
		NewSweep.AddSlice(0, i, newSlicePoints);
	}

	Vector3d xyz = NewSweep.slices[0].pts[6].xyz;

	Vector3d xyz2 = NewSweep.ptCloud[0][6].xyz;

	std::cout << "There are " << NewSweep.ptCloud[0].size() << " pts in the first slice of our ptcloud" << std::endl;

	double c = Mult(std::vector<double>{ 1, 1, 1 }, std::vector<double>{ 1 });

	std::vector<double> &d = Mult(std::vector<double>{ 1, 1, 1 }, 2.0);
	std::vector<double> &e = Mult(2.0, std::vector<double>{ 1, 1, 1 });

	double a = 5;

	double f = Mult(d, e);

	std::vector<double> g = Divide(d, 3.0);

	double h =  Dist(g);

	double hh = Dist(e, g);

	std::vector<double> aa = Divide(e, 3.0);

	std::vector<std::vector<double>> unsortedVec, sortedVec;

	for (int i = 0; i < 16; i++)
	{
		unsortedVec.push_back({ (double)rand(), (double)i });
	}

	sortedVec = unsortedVec;

	MergeSort(sortedVec);

	std::vector<std::vector<double>> testSlice;

	for (double i = 0; i < 100; i++)
	{
		switch((int)i/25)
		{
		case 0:
			testSlice.push_back({ 50,0,i / 10 });
		case 1:
			testSlice.push_back({ 50 + i, 0, i / 10 });
		case 2:
			testSlice.push_back({ 50 + 50 - i, 0, i / 10 });
		case 3:
			testSlice.push_back({ 50 + 25, 0, i / 10 });
		}
	}


	Sweep testSweep();

	return 0;
}