#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#ifdef _OPENMP
#include <omp.h>
#endif


int n, m, p;
float *a, *b, *c;
int nt;


double get_time() {
	struct timespec tiempo;
	double sec;
;
	if (clock_gettime(CLOCK_REALTIME, &tiempo)==0) {
		sec=tiempo.tv_sec+tiempo.tv_nsec/(double) 1000000000.0;
    		return sec;
	} else {
		printf("Obtener tiempo: NO HA HABIDO RESPUESTA DE GETTIME\n");
		return 0;
	}
}

void* mult(void *core_id) {
	
 	int i,j,k,x;
	float rtemp;
	int total = n*m;
	int core = (int)core_id;
	for (x = core * total/nt; x < (core + 1) * total/nt; x++) {
		i = x / m;
		j = x % m;
		if (j == 0) {	rtemp = 0; }
		for(k=0;k<p;k++){
			rtemp += a[i*p+k] * b[k*m+j];
		}
		c[(i*m)+j] = rtemp;
	}
	return;
}


int main(int argc, char* argv[]){
	int i, j, k;
	for (k = 2; k <= 8; k*=2) {
		printf("---------------------\n");
		printf("Num_threads = %d\n", k);
		printf("---------------------\n");
		nt = k;		
		for (i = 1; i < 4; i++) {

			switch (i){

			case 0:
				n=10;
				m=10;
				p=10;
				break;

			case 1:
				n=8000000;
				m=8;
				p=8;
				printf("A: 8000000x8 * 8x8: ");
				break;

			case 2:
				n=8;
				m=8000000;
				p=8;
				printf("B: 8x8000000 * 8000000x8: ");
				break;

			case 3:
				n=800;
				m=800;
				p=800;
				printf("C: 800x800 * 800x800: ");
				break;
			default:
				printf("Error: Debe introducir un valor entre 0 y 3\n");
				exit(1);
				break;
			}

			a = (float*)malloc(n*p*sizeof(float));
			b = (float*)malloc(p*m*sizeof(float));
			c = (float*)malloc(n*m*sizeof(float));

			if(a == NULL || b == NULL || c == NULL){
				printf("Alguna matriz no tiene memoria asignada");
				return -1;
			}
			
			double start, end, total = 0.0;
			pthread_t threads[nt];
			int try;
			for (try = 0; try < 3; try++) {
				start = get_time();
				int j;
				for (j = 0; j < nt; j++) {
					pthread_create(&threads[j], NULL, mult, (void *)(j));
				}
				void* dummy;
				for (j = 0; j < nt; j++) {
					pthread_join(threads[j], NULL);
				}
				end = get_time();
				total += (end - start);
			}
			printf("%lf\n", (total/3));
			
		}
	}
	exit(0);
}
