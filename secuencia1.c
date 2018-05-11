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
	
 	int i,j,k;
	float rtemp;
	//double start = get_time();
	int core = (int)core_id;
	for(i = core * n/nt ; i < (core + 1) * n/nt ; i++){
		for(j=0;j<m;j++){
			rtemp = 0;
			for(k=0;k<p;k++){
				rtemp += a[i*p+k] * b[k*m+j];
			}	
			c[(i*m)+j] = rtemp;
		}
	}
	//double end = get_time();
	//printf ("%lf\n", (end-start));
	return;
}


int main(int argc, char* argv[]){

	int i, j, k;
	int count;
	for (k = 2; k <= 8; k*=2) {
		printf("---------------------\n");
		printf("Num_threads = %d\n", k);
		printf("---------------------\n");
		nt = k;		
		for (count = 0; count <=20; count++) {
			int orden = count*100;
			printf ("Orden %d: ", orden);

			n = orden;
			m = orden;
			p = orden;

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
