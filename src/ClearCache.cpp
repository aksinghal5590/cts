#include <cstdlib>
#include <ctime>

int main() {
	int size = 12*1024*1024;
	int *a = new int[size]();

	srand(time(0));
	for(int i = 0; i < size; i++) {
		a[i] = (rand() %1000) + 1000;
	}
	for(int i = 0; i < size; i++) {
		a[i]--;
	}
	return 0;
}