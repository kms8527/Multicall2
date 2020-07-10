#include "Multicall.h"

int main() {
	int n; //node number
	int windowSize = 1000;
	int capacity = 3;
	vector<unsigned char> customer_on_board;
	cout << "�°� �� �Է� : ";
	cin >> n;
	
	Multicall M(n,windowSize);
	vector<double> score = M.getscore();
	vector<unsigned char> Visit_Order;
	for (int i = 0; i < n; i++) {
		cout<<score[i] << endl;;
	}
	Mat time_input = M.getdistanceandscore(); //�����̰� ���ͽ�Ʈ��� ���ߴٰ� ����.
	
	
	Visit_Order=M.fullsearch(score,time_input, capacity, customer_on_board);
	M.drawline(Visit_Order);
	M.plot(score);
	return 0;
}