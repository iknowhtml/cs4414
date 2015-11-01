#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define BUFF_SIZE 8192

pthread_mutex_t mutex;
pthread_cond_t condition;
pthread_attr_t attr;

int* array;
int count;
int size;
int n = 0;

int cmp(const void* a, const void* b){
	return *(int*)a - *(int*)b;
}

typedef struct{
	int index;
	int length;
} params;

params* parameters(int i, int l){
	params* p = (params*) malloc(sizeof(params));
	p -> index = i;
	p -> length = l;
	return p;
}

void barrier(){	
	pthread_mutex_lock(&mutex);
	count--;
	if(count == 0){
		pthread_cond_broadcast(&condition);
	}
	else{
		pthread_cond_wait(&condition, &mutex);
	}

	pthread_mutex_unlock(&mutex);
}

void* sort(void* p){
	qsort(array + ((params*)p) -> index, ((params*) p) -> length, sizeof(int), cmp);
	barrier();
	pthread_exit(0);
}

int main(){
	FILE *fp;

	fp = fopen("indata.txt", "r");

	int value;
	size = 0;


	while(fscanf(fp, "%d", &value) > 0){
		size++;
	}

	rewind(fp);
8ii/s     
	int a[size];

	array = a;

	for(int i = 0; i < size; i++){
		fscanf(fp, "%d", &value);
		array[i] = value;
	}
	fclose(fp);
	
	pthread_attr_init(&attr);

	for(int i = 0; i < log2(size); i++){
		count = size/pow(2, i + 1);
		pthread_t workers[count];

		int threads = count;
		for(int j = 0; j < threads; j++){
			pthread_create(&workers[j], &attr, sort, parameters(pow(2, i + 1) * j, pow(2, i + 1)));
		}

		while(count > 0);
	}

	for(int i = 0; i < size; i++){
		printf("%d ", array[i]);
	}
	printf("\n");
	
	return 0;
}