#include "Multicall.h"


Multicall::Multicall(int n,int size) :n(n) {
	
	img = Mat(size, size, CV_8UC3, Scalar(255, 255, 255));
	
	srand((unsigned int)time(NULL));

	for (int i = 0; i < n; i++) {
		customer_s.push_back(Point2i(rand() % img.rows, rand() % img.cols));
		customer_d.push_back(Point2i(rand() % img.rows, rand() % img.cols));
	}
	mypos = Point2i(img.rows/2, img.cols/2);
	//mypos = Point2i((customer_d[0].x+customer_s[0].x)/2, (customer_d[0].y + customer_s[0].y) / 2);                 //내 현재 위치
	//mypos = Point2i(0, 0);
};

void Multicall::plot(const vector<double>& score) {
	int rectangle_length=10;
	
	for (int i = 0; i < n; i++) {
		int color_R = rand() % 255;
		int color_G = rand() % 255;
		int color_B = rand() % 255;
		
		//string str_time = to_string(//남은 시간 
		string str = to_string((int)score[i]);
		string str2 = to_string(i+1);
		string st = "S"+to_string(i+1)+": ";
		line(img, Point(mypos.x - 5, mypos.y - 5), Point(mypos.x + 5, mypos.y + 5), CV_RGB(255, 0, 0),4);
		line(img, Point(mypos.x - 5, mypos.y + 5), Point(mypos.x + 5, mypos.y - 5), CV_RGB(255, 0, 0),4);

		circle(img, customer_s[i], 10, Scalar(color_B, color_G, color_R), -1); // 참고로 스칼라는 BGR 순임.
		rectangle(img, customer_d[i] - Point2i(rectangle_length, rectangle_length), customer_d[i] + Point2i(rectangle_length, rectangle_length),Scalar(color_B, color_G,color_R),-1);
		putText(img, st+str , customer_d[i], FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 0, 0));
		putText(img, str2,customer_s[i], FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 0, 0));
		
	}
	imshow("img", img);
	waitKey(0);
}

vector<double> Multicall::getscore() {
	vector<double> score;
	srand((unsigned int)time(NULL));
	for (int i = 0; i < n; i++) {
		double distance_s_to_d = sqrt(pow(customer_s[i].x - customer_d[i].x, 2) + pow(customer_s[i].y - customer_d[i].y, 2)); //승객 출발지 ->목적지 거리 = 점수라 가정
		score.push_back((distance_s_to_d+rand()%30)/5);
	}
	return score;
}

Mat Multicall::getdistanceandscore() {
	double velocity = 5;
	Mat D(2 * n + 1, 2 * n + 1, CV_64FC1, Scalar(0));
	
	vector<Point2i> total_node = customer_s;
	total_node.insert(total_node.end(), customer_d.begin(), customer_d.end());
	//vector<double> time;  //예상 소요 시간 저장
	
	for (int j = 0; j < 2*n; j++)
	{
		D.at<double>(0, j+1) = sqrt(pow(mypos.x - total_node[j].x, 2) + pow(mypos.y - total_node[j].y, 2))               /10; //0행 모든 열 초기화
		D.at<double>(j+1, 0) = sqrt(pow(mypos.x - total_node[j].x, 2) + pow(mypos.y - total_node[j].y, 2))               /10; //0열 모든 행 초기화

	}

	for (int i = 0; i < 2 * n; i++) {
		//time.push_back(distance_s_to_d / velocity);
		for (int j = i+1; j < 2*n; j++) { //상 삼각요소들을 채움
				D.at<double>(i+1, j+1) = sqrt(pow(total_node[i].x - total_node[j].x, 2) + pow(total_node[i].y - total_node[j].y, 2))        /10;
		}
		for (int j = 0; j < i; j++) {//대각요소 아래쪽 삼각요소들을 채움
			D.at<double>(i+1,j+1) = sqrt(pow(total_node[i].x - total_node[j].x, 2) + pow(total_node[i].y - total_node[j].y, 2))        /10;
		}
		
	}
	return D;
}

bool Multicall::isfeasible(int n, int visit_node, const vector<unsigned char> customer_on_board, int capacity) {

	if (visit_node <= n && customer_on_board.size() < capacity) { // 방문할 노드가 출발 노드 + 승차공간이 있는 경우 =>방문 가능
		for (int i = 0; i < customer_on_board.size(); i++) {
			if (customer_on_board[i] == visit_node)
				return false;

		}
		return true;
	}
	else if (visit_node > n) { // 방문할 노드가 도착 노드인 경우
		for (int i = 0; i < customer_on_board.size(); i++) //+도착노드에 내릴 승객이 있는경우  =>방문 가능
			if (customer_on_board[i] +n== visit_node) {
				return true;
			}
	}
	
	return false; // 나머지는 불가능
}

//openset = sub_openset들의 집합 , visit node= 방문 노드, n = 승객 수

void Multicall::openset_update(vector<openset_data>& openset, const unsigned char& visit_node, const int& n, const int capacity, queue<q_data>& q ) {
	
	vector<unsigned char> new_open_subset = openset.front().sub_openset;
	vector<unsigned char> new_customer_on_board = openset.front().customer_on_board;
	vector<unsigned char>::iterator it = find(new_open_subset.begin(), new_open_subset.end(), visit_node);
	unsigned char index = distance(new_open_subset.begin(), it);
	new_open_subset.erase(new_open_subset.begin() + index);

	
	//    출발노드이고  && 1. 이제 만석 2. 아직 만석 x
	if (visit_node <= n){
		if (openset.front().customer_on_board.size() + 1 == capacity) {
			new_open_subset.push_back(n + visit_node);
			new_customer_on_board.push_back(visit_node);
			for (int i = 0; i < new_open_subset.size(); i++) {
				if (new_open_subset[i] <= n)
					new_open_subset.erase(new_open_subset.begin() + i);
			}
		}
		else {
			new_open_subset.push_back(n + visit_node);
			new_customer_on_board.push_back(visit_node);
		}
		
	}
	// 목적 노드 && 1. 만석이었던 경우 2.만석이 아니었던 경우(생각할 필요 x)
	else {
		//일단 승객을 차에서 내림
		for (int i = 0; i < new_customer_on_board.size(); i++) {
			if (new_customer_on_board[i] == visit_node-n)
				new_customer_on_board.erase(new_customer_on_board.begin() + (i)); //i 번째 원소 지움
		}
		if (openset.front().customer_on_board.size() == capacity) {
			vector<unsigned char> unvisited_starting_points;
			
			for (int j = 1; j <= n; j++) {
				bool flag = true;
				for (int i = 0; i < q.front().pre_nodes.size(); i++) {
					if (q.front().pre_nodes[i] == j) {
						flag=false;
						break;
					}
					
				}
				if (flag==true)
					unvisited_starting_points.push_back(j);
			}
			for (int i = 0; i < new_customer_on_board.size(); i++) {
				unvisited_starting_points.push_back(new_customer_on_board[i]+n);
			}
			return openset.push_back({ unvisited_starting_points, new_customer_on_board });
		}
	}
	openset.push_back({ new_open_subset, new_customer_on_board });
}

//void Multicall::isvisit_update(vector<vector<bool>>& isvisit, const unsigned char& visit_node, const int& n) {
//	vector<bool> new_isvisit_subset = isvisit.front();
//	new_isvisit_subset[visit_node] = true;
//	isvisit.push_back(new_isvisit_subset);
//	isvisit.erase(isvisit.begin());
//}


vector<unsigned char> Multicall::fullsearch(const vector<double>& score,const Mat& D_time,const int& capacity, vector<unsigned char>& customer_on_board) {
	unsigned char N = 2*3; // predict step
	vector<unsigned char> Visit_Order;
	int length = 2 * n + 1; //total node number
	vector<openset_data> openset;
	vector<unsigned char> sub_openset_initial;
	queue<q_data> q;
	vector<unsigned char> initial_pre_node{customer_on_board};


	int lll=0;
	// isvisit array initialization
	// 0 : no visit , 1: visit
	
	for (int i = 1; i < length; i++) { // openset은 다음을 포함 : 1~n 노드 + 승객 탑승중(n+i)
		if (isfeasible( n, i, customer_on_board, capacity)) {
			sub_openset_initial.push_back(i);
		}
	}
	
	q.push({ 0, 0, 0,initial_pre_node});
	openset.push_back({ sub_openset_initial, customer_on_board });

	while (1) {
		
		if (q.front().pre_nodes.size() ==  N) {
			/*priority_queue<int,vector<int>,less<int>> Select;*/
			double Best = 0;
			double Best_candidate = 0;
			while (q.empty() != true) {
				Best_candidate = q.front().score / q.front().time;
				if (Best < Best_candidate) {
					Best = Best_candidate;
					Visit_Order = q.front().pre_nodes;
				}
				if (Visit_Order.empty() == true)
					cout << "d";
				q.pop();
			}
			if (initial_pre_node.empty() != true) {
				for (int i = 0; i < initial_pre_node.size(); i++) {
					Visit_Order.erase(Visit_Order.begin()+i);
				}
			}
			cout << "경로 명단 : " << endl;
			for (int i = 0; i < Visit_Order.size(); i++) {
				cout << (int)Visit_Order[i] << " -> ";
			}
			cout << "종료" << endl;
			return Visit_Order;
		}

		////////////////////////////

		for ( int i=0;openset.empty()!=true && i < openset.front().sub_openset.size(); i++){
			double cur_score = q.front().score; //현재 점수
					
			double cur_time = q.front().time + D_time.at<double>(q.front().node, openset.front().sub_openset[i]);   //이번에 방문할 노드 : openset.front().sub_opoenset[i]
			
			if (openset.front().sub_openset[i] > n) { //방문노드가 도착노드이면 점수 추가
				cur_score += score[openset.front().sub_openset[i] - (n + 1)];
			}
			vector<unsigned char> new_trajectory = q.front().pre_nodes;

			new_trajectory.push_back(openset.front().sub_openset[i]); //큐에 넣기전 사전 작업(이전 경로에 내 경로를 넣어줌)
			q.push({ openset.front().sub_openset[i], cur_score, cur_time, new_trajectory});  //큐에 넣음(방문 시도한 것 저장)
			
			//방문 노드        ,현재 점수,  현재 걸리는 시간 (subopenset의 i번째 요소에 접근)
			openset_update(openset, openset.front().sub_openset[i], n, capacity,q);  //방문했을 때의 openset 변화를 갱신 + 이전 openset 제거
			
		}
		lll++;
		cout << lll << endl;
		
		q.pop();
		if(openset.empty() != true)
			openset.erase(openset.begin());
		/////////////////////////////
	}
}

void Multicall::drawline(vector<unsigned char> Visit_Order) { // result : 1 channel = 1 or 0 2 channel = vi
	if (Visit_Order[0] <= n) {
		line(img, mypos, customer_s[Visit_Order[0] - 1], Scalar(255, 0, 0), 3);
	}
	else if (Visit_Order[0]>n) {
		line(img, mypos, customer_d[Visit_Order[0] - n-1], Scalar(255, 0, 0), 3);
	}

	for (int i = 0; i < Visit_Order.size()-1; i++) {
		if (Visit_Order[i] <= n && Visit_Order[i+1]<= n){ //출발 ->출발
			line(img, customer_s[Visit_Order[i] - 1], customer_s[Visit_Order[i + 1] - 1], Scalar(255, 0, 0), 3);
		}
		else if (Visit_Order[i] <= n && Visit_Order[i + 1] > n){ //출발 ->도착
			line(img, customer_s[Visit_Order[i] - 1], customer_d[Visit_Order[i + 1] - n - 1], Scalar(255, 0, 0), 3);

		}
		else if (Visit_Order[i] > n && Visit_Order[i + 1] <= n){  //도착 ->출발
			line(img, customer_d[Visit_Order[i] - n - 1], customer_s[Visit_Order[i + 1] - 1], Scalar(255, 0, 0), 3)	;
		}
		else {//도착 -> 도착
			line(img, customer_d[Visit_Order[i] - n - 1], customer_d[Visit_Order[i + 1] -n - 1], Scalar(255, 0, 0), 3);

		}
	}
};

