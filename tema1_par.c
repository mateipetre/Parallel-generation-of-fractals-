/*
 * APD - Tema 1
 * Octombrie 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int P;
pthread_barrier_t barrier;

// structura pentru un numar complex
typedef struct _complex {
	double a;
	double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
	int is_julia, iterations;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia;
} params;

//variabile utilizate pentru calculul matricelor (multimilor)
params par1, par2;
int width1, height1, width2, height2;
int **result1, **result2;

// citeste argumentele programului
void get_args(int argc, char **argv)
{
	if (argc < 6) {
		printf("Numar insuficient de parametri:\n\t"
				"./tema1 fisier_intrare_julia fisier_iesire_julia "
				"fisier_intrare_mandelbrot fisier_iesire_mandelbrot nr_threads\n");
		exit(1);
	}

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	P = atoi(argv[5]);
}

// citeste fisierul de intrare
void read_input_file(char *in_filename, params* par)
{
	FILE *file = fopen(in_filename, "r");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de intrare!\n");
		exit(1);
	}

	fscanf(file, "%d", &par->is_julia);
	fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	fscanf(file, "%lf", &par->resolution);
	fscanf(file, "%d", &par->iterations);

	if (par->is_julia) {
		fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
	}

	fclose(file);
}

// scrie rezultatul in fisierul de iesire
void write_output_file(char *out_filename, int **result, int width, int height)
{
	int i, j;

	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		return;
	}

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
{
	int **result;
	int i;

	result = malloc(height * sizeof(int*));
	if (result == NULL) {
		printf("Eroare la malloc!\n");
		exit(1);
	}

	for (i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	return result;
}

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}

// ruleaza algoritmii Julia si Mandelbrot dintr-o singura
// functie de thread
void *run_julia_mandelbrot(void *arg)
{
	int w, h, i;
	int thread_id = *(int *)arg;

	// se paralelizeaza calculul multimii Julia
	// impart bucla corespondenta inaltimii imaginii la cele P thread-uri
	// se calculeaza index-ul de start si end pt fiecare thread
	int start = thread_id * (double)height1 / P;
	int end;
	if(((thread_id + 1) * (double)height1 / P) <= height1)
		end = (thread_id + 1) * (double)height1 / P;
	else
		end = height1;

	for (w = 0; w < width1; w++) {
		for (h = start; h < end; h++) {
			int step = 0;
			complex z = { .a = w * par1.resolution + par1.x_min,
							.b = h * par1.resolution + par1.y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par1.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + par1.c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par1.c_julia.b;

				step++;
			}
			result1[h][w] = step % 256;
		}
		// folosesc o bariera pt a sincroniza thread-urile
		pthread_barrier_wait(&barrier);
	}
	// transforma rezultatul din coordonate matematice in coordonate ecran
	// paralelizez si aceasta operatie, tot prin impartirea la cele P thread-uri
	start = thread_id * (double)(height1 / 2) / P;
	if((thread_id + 1) * (double)(height1 / 2) / P <= (height1 / 2))
		end = (thread_id + 1) * (double)(height1 / 2) / P;
	else
		end = height1 / 2;

	for (i = start; i < end; i++) {
		int *aux = result1[i];
		result1[i] = result1[height1 - i - 1];
		result1[height1 - i - 1] = aux;
	}
	// se paralelizeaza calculul multimii Mandelbrot
	// impart bucla corespondenta inaltimii imaginii la cele P thread-uri
	// se calculeaza index-ul de start si end pt fiecare thread
	start = thread_id * (double)height2 / P;
	if(((thread_id + 1) * (double)height2 / P) <= height2)
		end = (thread_id + 1) * (double)height2 / P;
	else
		end = height2;

	for (w = 0; w < width2; w++) {
		for (h = start; h < end; h++) {
			complex c = { .a = w * par2.resolution + par2.x_min,
							.b = h * par2.resolution + par2.y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par2.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result2[h][w] = step % 256;
		}
		// folosesc o bariera pt a sincroniza thread-urile
		pthread_barrier_wait(&barrier);
	}
	// transforma rezultatul din coordonate matematice in coordonate ecran
	// paralelizez si aceasta operatie, tot prin impartirea la cele P thread-uri
	start = thread_id * (double)(height2 / 2) / P;
	if(((thread_id + 1) * (double)(height2 / 2) / P) <= (height2 / 2))
		end = (thread_id + 1) * (double)(height2 / 2) / P;
	else
		end = height2 / 2;
	for (i = start; i < end; i++) {
		int *aux = result2[i];
		result2[i] = result2[height2 - i - 1];
		result2[height2 - i - 1] = aux;
	}
	pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
	//declarare necesara pentru a implementa calculul paralel
	pthread_t tid[P];
	int thread_id[P];
	int i;
	get_args(argc, argv);
	// initializare bariera
	int b = pthread_barrier_init(&barrier, NULL, P);
	if(b != 0) {
		printf("Eroare la initializarea barierei");
		exit(-1);
	}
	// se citesc argumentele programului
	read_input_file(in_filename_julia, &par1);
	width1 = (par1.x_max - par1.x_min) / par1.resolution;
	height1 = (par1.y_max - par1.y_min) / par1.resolution;
	result1 = allocate_memory(width1, height1);

	read_input_file(in_filename_mandelbrot, &par2);
	width2 = (par2.x_max - par2.x_min) / par2.resolution;
	height2 = (par2.y_max - par2.y_min) / par2.resolution;
	result2 = allocate_memory(width2, height2);

	// se creeaza cele P thread-uri
	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		pthread_create(&tid[i], NULL, run_julia_mandelbrot, &thread_id[i]);
	}

	// se asteapta cele P thread-uri
	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	// se afiseaza multimile Julia si Mandelbrot
	write_output_file(out_filename_julia, result1, width1, height1);
	write_output_file(out_filename_mandelbrot, result2, width2, height2);

	// se elibereaza memoria
	free_memory(result1, height1);
	free_memory(result2, height2);

	// se distruge bariera creata si folosita la sincronizare
	b = pthread_barrier_destroy(&barrier);
	if(b != 0) {
		printf("Eroare la dezalocarea barierei");
		exit(-1);
	}
	pthread_exit(NULL);
	return 0;
}
