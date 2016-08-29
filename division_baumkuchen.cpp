#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <ciso646>
#include <iterator>
#include <map>

using namespace std;

#define REP(i,n) for(int i=0;i<n;++i)
#define ALL(x) (x).begin(),(x).end()

struct Polar {
	double r, theta, z;
	Polar(double r_, double theta_, double z_ = 0.0) :r(r_), theta(theta_), z(z_) {}
	Polar() {}
};

struct Point{
	double x,y,z;
	Point(double x_, double y_, double z_):x(x_), y(y_), z(z_){}
	Point() {}
	bool operator < (const Point& right) const  {
		if (x < right.x) return true;
		if (y < right.y) return true;
		return false;
	}
	void operator = (const Point& right) {
		x = right.x;
		y = right.y;
		z = right.z;
	}
	Polar GetPolar() {
		Polar res;
		res.r = sqrt(x*x + y*y);
		if (y == 0) res.theta = 0.0;
		else res.theta = atan(-y/x)*180/3.141592654;
		res.z = z;
		return res;
	}
};

// function family for Getting cellPoints
class GetCellPoints {
private:
	vector<double> inputFromFile(ifstream fin) {
		int N; fin >> N;
		vector<double> res(N); for (auto &a : res) { fin >> a;}
		return res;
	}

	vector<double> inputFromFile(string filename) {
		return inputFromFile(ifstream(filename));
	}

	vector<int> ownerFromFile(string filename) {
		ifstream fin(filename);
		int N; fin >> N;
		vector<int> res(N); for (auto &a : res) fin >> a;
		return res;
	}

	vector<Point> pointFromFile(string filename) {
		ifstream fin(filename);
		int N; fin >> N;
		vector<Point> res(N);
		for (auto &a : res) {
			fin >> a.x >> a.y >> a.z;
		}
		return res;
	}

	vector<vector<int> > faceFromFile(string filename) {
		ifstream fin(filename);
		int N; fin >> N;
		vector<vector<int> > res(N);
		for (auto &a : res) {
			int num; fin >> num;
			REP(i, num) {
				int b; fin >> b; a.push_back(b);
			}
		}
		return res;
	}

	vector<Point> GetPoints(const vector<int>& face, const vector<Point> &points) {
		vector<Point> res;
		for (auto &a : face) {
			res.push_back(points[a]);
		}
		return res;
	}

public:
	vector<vector<Point> > operator()(int argc, char* argv[]) {
		string dirName;
		if (argc > 4) {
			dirName = argv[4];
			if (dirName.back() != '/') dirName += "/";
		}
		vector<int> owners = ownerFromFile(dirName+"owner");
		vector<double> neighbours = inputFromFile(dirName+"neighbour");
		//points[i]: point
		vector<Point> points = pointFromFile(dirName+"points");
		//faces[i]: face: group of points(index number)
		vector<vector<int> > faces = faceFromFile(dirName+"faces");
		//facePoints[i]: face: group of points (coordinate)
		vector<vector<Point> > facePoints(faces.size());
		REP(i, facePoints.size()) facePoints[i] = GetPoints(faces[i], points);
		//cellPoints[i]: cell : group of points (coodinate)
		vector<vector<Point> > cellPoints(*max_element(ALL(owners)) + 1);
		REP(i, owners.size()) {
			int owner = owners[i];
			cellPoints[owner].insert(cellPoints[owner].end(), facePoints[i].begin(), facePoints[i].end());
		}
		return cellPoints;
	}
};

void PrintCellNum(const vector<vector<Point> > &cellPoints, const map<tuple<int, int, int>, vector<int> >& cellNum) {
	cout << setprecision(10);
	vector<int> res(cellPoints.size());
	{
		int cnt = 0;
		for (auto &a : cellNum) {
			for (auto &i : a.second) {
				res[i] = cnt;
			}
			++cnt;
		}
	}

	cerr << cellNum.size() << endl;
	cout << res.size() << endl;
	cout << "(" << endl;
	for (auto r : res) {
		cout << r << endl;
	}
	cout << ")" << endl;
}

const double EPS = 0.0001;

// argv[0]: filename
// argv[1]: divr
// argv[2]: divtheta
// argv[3]: dirname(optional)
int main(int argc, char* argv[]) {

	if (argc <= 3) {
		cerr << "./a.out divr divtheta cutz [dirname(optional)]" << endl;
		return 0;
	}
	//number of division in the r-direction
	int rDiv = atoi(argv[1]);
	//number of division in the theta-direction
	int thetaDiv = atoi(argv[2]);
	//number of cut in the z-direction
	double cutz = atof(argv[3]);
	vector<vector<Point> > cellPoints = GetCellPoints()(argc, argv);

	//search for max of r, theta
	double maxr = 0;
	double maxtheta = -1e30;
	double mintheta = 1e30;
	REP(i, cellPoints.size()) {
		for (Point p : cellPoints[i]) {
			Polar po = p.GetPolar();
			maxr = max(maxr, po.r);
			maxtheta = max(maxtheta, po.theta);
			mintheta = min(mintheta, po.theta);
		}
	}
	// cerr << "maxtheta:" << maxtheta << endl;
	// cerr << "mintheta:" << mintheta << endl;

	// divition r and theta, and classification
	map<tuple<int, int, int>, vector<int> > cellNum;
	REP(i, cellPoints.size()) {
		// max r and theta in this cell points
		double localMaxr = 0.0;
		double localMaxtheta = 0.0;
		double localMinz = 1e30;
		for (Point p : cellPoints[i]) {
			Polar po = p.GetPolar();
			localMaxr = max(localMaxr, po.r);
			localMaxtheta = max(localMaxtheta, po.theta - mintheta);
			localMinz = min(localMinz, po.z);
		}
		// cout << "z:" << localMinz << " r:" << localMaxr << " theta:" << localMaxtheta << endl;;
		double divedR = maxr / rDiv + EPS; //dived r
		double divedTheta = (maxtheta - mintheta) / thetaDiv + EPS; // dived theta
		int isCutz = (localMinz < cutz + EPS);

		cellNum[make_tuple(int(localMaxr / divedR), (int((double)localMaxtheta / (double)divedTheta + 0.5) == thetaDiv ? 0 : int((double)localMaxtheta/(double)divedTheta + 0.5)), isCutz)].push_back(i);

	}

	PrintCellNum(cellPoints, cellNum);

	return 0;
}
