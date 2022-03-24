#include "philo.h"

int is_all_digits(const char *arg)
{
    const char *head;

    head = arg;
    if (arg == NULL)
        return 1;
    while(*arg != '\0')
    {
        if (*arg < '0' || *arg > '9')
        {
            printf("Agument: %s\n", head);
            return 1;
        }
        arg++;
    }
    return (0);
}

int check_args(int argc, const char *argv[])
{
    int i;

    i = 0;
    if (argc < 5 || argc > 6)
        return (1);
    while(++i < argc)
    {
        if (is_all_digits(argv[i]) != 0)
            return (1);
    }
    return (0);
}

void fill_prog(t_table *table, int argc, const char *argv[])
{
	memset(table, 0, sizeof(t_table));
	table->number_of_philosophers = ft_atoi(argv[1]);
	table->time_to_die = ft_atoi(argv[2]);
	table->time_to_eat = ft_atoi(argv[3]);
	table->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		table->number_of_eat = ft_atoi(argv[5]);
	else
		table->number_of_eat = 0;
}

int main(int argc, const char *argv[])
{
    t_table prog;

    if (check_args(argc, argv) != 0)
    {
        printf("Error arguments\n");
        return (-1);
    }
	fill_prog(&prog, argc, argv);
    return 0;
}

//void wait_thread (void);
//void* thread_func (void*);
//
//int main (int argc, char *argv[], char *envp[]) {
//    pthread_t thread;
//    if (pthread_create(&thread, NULL, thread_func, NULL))
//        return 1;
//    for (unsigned int i = 0; i < 20; i++) {
//        puts("a");
//        //wait_thread();
//    }
//    if (pthread_join(thread,NULL))
//        return 1;
//    return 1;
//}
//
//void wait_thread (void) {
//    time_t start_time = time(NULL);
//    while(time(NULL) == start_time) {}
//}
//
//void* thread_func(void* vptr_args) {
//    for (unsigned int i = 0; i < 20; i++) {
//        fputs("b\n",stderr);
//        //wait_thread();
//    }
//    return NULL;
//}


