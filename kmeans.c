#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#define int_INFINITY 2147483647
#define EPSILON 0.001


struct cord
{
    double value;
    struct cord *next;
};
struct vector
{
    struct vector *next;
    struct cord *cords;
};
typedef struct vector vector;
typedef struct cord cord;


int input_to_Llist(struct vector**);
void free_Llist(struct vector**);
void cluster_assign(double**, int*, vector*,int, int, int);
int update_centroids(double**, int*, vector*,int,int,int);
double euclidean_distance(double*, cord*, int);
double euclidean_distance_new(double*, double*, int);
void handle_memory_error(vector **, double **, int , int*) ;
void input_validation(char* );



int main(int argc, char *argv[]) {
    vector *head = NULL, *current_vec = NULL;
    cord *current_cord = NULL;
    int k, iter,n,d = 0, i = 0, j = 0, flag = 1, t = 0;
    double** centroids;
    int  *cluster;

    input_validation(argv[1]);
    k = atoi(argv[1]);
    if (argc == 2)
    {
        iter = 200;
    }
    else
    {
        input_validation(argv[2]);       
        iter = atoi(argv[2]);
        if(!(iter>1 && iter<1000))
        {
            printf("Invalid maximum iteration!");
            return 1;
        }
    }

   n= input_to_Llist(&head);

    if (!(k>1 && k<n))
    {
        printf("Invalid number of clusters!");
        return 1;
    }
    current_cord = head->cords;
    while (current_cord != NULL)
    {
        d++;
        current_cord = current_cord->next;
    }
    centroids = calloc( k, sizeof(double*));
    if (!centroids) handle_memory_error(&head, NULL, 0, NULL);
    current_vec = head;
    for (i = 0; i < k; i++)
    {
        current_cord = current_vec->cords;
        centroids[i] = calloc(d, sizeof(double));
         if (!centroids[i]) handle_memory_error(&head, centroids, i, NULL);
        for ( j = 0; j < d; j++)
        {
            centroids[i][j] = current_cord->value;
            current_cord = current_cord->next;
            
        } 
        current_vec = current_vec->next;

    }
    cluster = calloc(n, sizeof(int));
    if (!cluster) handle_memory_error(&head, centroids, k, NULL);
    while (flag &&(t<iter))
    {
        cluster_assign(centroids, cluster, head, n, d, k);
        flag = update_centroids(centroids, cluster, head, n, d, k);
        t++;
    }
    for (i = 0; i < k; i++)
    {
        for(j = 0; j < d; j++)
        {
            if(j == d-1)
            {
                printf("%.4f",centroids[i][j]);
            }
            else
            {
                printf("%.4f,",centroids[i][j]);
            }
        }
        printf("\n");
    }
    for (i = 0; i < k; i++)
    {
        free(centroids[i]);
    }
    free(centroids);
    free_Llist(&head);
    free(cluster);

    return 0;
    
}

int input_to_Llist(struct vector** head_vec)
{
    double n;
    int i = 0;
    char c;
    vector *curr_vec;
    cord *head_cord, *curr_cord;

    
    head_cord = malloc(sizeof(struct cord));
    if (!head_cord) handle_memory_error(NULL, NULL, 0, NULL);
    curr_cord = head_cord;
    curr_cord->next = NULL;
    curr_cord->value = 0.0;

    *head_vec = malloc(sizeof(struct vector));
    if (!*head_vec)
    {
        free(head_cord);
        handle_memory_error(NULL, NULL, 0, NULL);
    }
    curr_vec = *head_vec;
    curr_vec->next = NULL;
    curr_vec->cords = NULL;
   while (scanf("%lf%c", &n, &c) == 2)
    {
        if (c == '\n')
        {
            i++;
            curr_cord->value = n;
            curr_vec->cords = head_cord;
            curr_vec->next = malloc(sizeof(struct vector));
            if (!curr_vec->next) handle_memory_error(head_vec, NULL, 0, NULL);

            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            curr_vec->cords = NULL;
            head_cord = malloc(sizeof(struct cord));
            if (!head_cord) handle_memory_error(head_vec, NULL, 0, NULL);

            curr_cord = head_cord;
            curr_cord->next = NULL;
            curr_cord->value = 0.0;
            continue;
        }

        curr_cord->value = n;
        curr_cord->next = malloc(sizeof(struct cord));
        if (!curr_cord->next) handle_memory_error(head_vec, NULL, 0, NULL);

        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
        curr_cord->value = 0.0;
    }
    free(curr_cord);
    return i;

}


void cluster_assign(double** centroids , int* cluster_assign, vector* head,int n, int d, int k)
{
    int i = 0, j = 0;
    double tmp, min = int_INFINITY;
    int mini = 0;
    for (i =0; i  < n; i++)
    {
        for( j = 0; j <k; j++)
        {
          tmp = euclidean_distance(centroids[j], head->cords, d);
          if (tmp < min)
          {
             min = tmp;
             mini = j;
          }
         
        }
        cluster_assign[i] = mini;
        head = head->next;
        min = int_INFINITY;
    }
}

double euclidean_distance(double* centroid , cord* cords, int d)
{
    int i = 0;
    double sum = 0;
    for (i = 0; i < d; i++)
    {
        sum += pow((cords->value - centroid[i]), 2);
        cords = cords->next;
    }
    sum = sqrt(sum);
    return sum;
}

int update_centroids(double** centroids , int* cluster_assign, vector* head,int n, int d, int k)
{
    int flag = 0;
    int i =0, j = 0;
    double** new_centroids;
    double delta = 0;
    vector *temp_head = NULL;

    new_centroids = calloc(k, sizeof(double*));
    if (!new_centroids) handle_memory_error(&head, centroids, k, cluster_assign);
    for (i = 0; i < k; i++)
    {
        new_centroids[i] = calloc(d+1, sizeof(double));
        if (!new_centroids[i]) handle_memory_error(&head, centroids, k, cluster_assign);
    }

    temp_head = head; 
    for (i = 0; i < n; i++) {
        cord *temp_cord = temp_head->cords;
        for (j = 0; j < d; j++) {
            new_centroids[cluster_assign[i]][j] += temp_cord->value;
            temp_cord = temp_cord->next;
        }
        new_centroids[cluster_assign[i]][d]++;
        temp_head = temp_head->next;
    }
    for( i = 0; i < k; i++)
    {
        for( j = 0; j < d; j++)
        {
            new_centroids[i][j] /= new_centroids[i][d];
        }
        delta = euclidean_distance_new(centroids[i], new_centroids[i], d);
        if (delta >= EPSILON)
        {
            flag = 1;
        }
        free(centroids[i]);
        centroids[i] = new_centroids[i];
    }
    free(new_centroids); 
    return flag;
}

double euclidean_distance_new(double* centroid , double* cords, int d)
{
    int i = 0;
    double sum = 0;
    for (i = 0; i < d; i++)
    {
        sum += pow((cords[i] - centroid[i]), 2);
    }
    sum = sqrt(sum);
    return sum;
}


void free_Llist(struct vector** head)
{
    vector *temp_vec = *head;
    vector *next_vec = NULL;
    cord *temp_cord = NULL;
    cord *next_cord = NULL;
    while (temp_vec != NULL)
    {
        next_vec = temp_vec->next;
        temp_cord = temp_vec->cords;
        while (temp_cord != NULL)
        {
            next_cord = temp_cord->next;
            free(temp_cord);
            temp_cord = next_cord;
        }
        free(temp_vec);
        temp_vec = next_vec;
    }
}

void handle_memory_error(vector **head, double **centroids, int k, int *cluster) {
    int i=0;
    printf("An Error Has Occurred\n");
    if (centroids) {
        for (i = 0; i < k; i++) {
            free(centroids[i]);
        }
        free(centroids);
    }
    if (cluster) {
        free(cluster);
    }
    if (head) {
        free_Llist(head);
    }
    exit(1);
}


void input_validation(char* input)
{
   int i=0;
        if (input[0] == '+')
        {
            i++;   
        }
        while(input[i] != '\0')
        {
            if(input[i] < '0' || input[i] > '9')
            {
                if  (input[i] == '.')
                {
                    i++;
                    if (input[i] != '0')
                    {
                        printf("An Error Has Occurred\n");
                        exit(1);
                    }
                    else
                    {
                        while(input[i] != '\0')
                        {
                           if (input[i] != '0')
                           {
                                 printf("An Error Has Occurred\n");
                                 exit(1);
                           }
                            i++;
                        }
                        break;
                    }
                }
                else
                {
                    printf("An Error Has Occurred\n");
                    exit(1);
                }

            }
            i++;
        }
}


