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
            return (ERROR);
        }
        arg++;
    }
    return (SUCCESS);
}

int check_args(int argc, const char *argv[])
{
    int i;

    i = 0;
    if (argc < 5 || argc > 6)
        return(ERROR);
    while(++i < argc)
    {
        if (is_all_digits(argv[i]) != 0)
            return (ERROR);
    }
    return(SUCCESS);
}

int fill_prog(t_table *table, int argc, const char *argv[])
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
	table->forks = (int *)malloc(sizeof(int) * table->number_of_philosophers);
	if (table->forks == NULL)
		return (ERROR);
	table->threads = (pthread_t *)malloc(sizeof(pthread_t) * table->number_of_philosophers);
	if (table->threads == NULL)
		return (ERROR);
	table->philos = (t_philo *)malloc(sizeof(t_philo) * table->number_of_philosophers);
	if (table->philos == NULL)
		return (ERROR);
	return (SUCCESS);
}

void philo_eat(t_philo *philo)
{
	struct timeval  time;

	gettimeofday(&time, NULL);
	printf("%d %d is eating\n", time.tv_usec, philo->number + 1);
}

void philo_thing(t_philo *philo)
{
	struct timeval  time;

	gettimeofday(&time, NULL);
	printf("%d %d is thinking\n", time.tv_usec, philo->number + 1);

}

void philo_sleep(t_philo *philo)
{
	struct timeval  time;

	gettimeofday(&time, NULL);
	printf("%d %d is sleeping\n", time.tv_usec, philo->number + 1);
}

void *working_philo(void *arg)
{
	t_philo *philo;

	philo = (t_philo *)arg;
	philo_eat(philo);
	philo_thing(philo);
	philo_sleep(philo);
	return (SUCCESS);
}

int start_philo(t_table *table)
{
	int i;
	int status;

	i = -1;
	status = 0;
	while(++i < table->number_of_philosophers)
	{
		table->philos[i].number = i;
		table->philos[i].table = table;
		status = pthread_create(&table->threads[i], NULL, working_philo, &table->philos[i]);
		if (status != 0)
			return (ERROR);
	}
	return (SUCCESS);
}

int join_philo(t_table *table)
{
	int	i;
	int	status;
	int	*return_status;

	i = -1;
	status = 0;
	return_status = 0;
	while(++i < table->number_of_philosophers)
	{
		status = pthread_join(table->threads[i], (void**)&(*return_status));
		if (status != 0)
			return (ERROR);
	}
	return (SUCCESS);
}

void free_data(t_table *table)
{
	free(table->forks);
	free(table->threads);
	free(table->philos);
}

int main(int argc, const char *argv[])
{
    t_table table;

    if (check_args(argc, argv))
    {
        printf("Error arguments\n");
        return (-1);
    }
	if (fill_prog(&table, argc, argv) != 0)
	{
		printf("Error malloc\n");
		return (-1);
	}
	if (start_philo(&table) == ERROR)
	{
		printf("Error create\n");
		return (-1);
	}
	if (join_philo(&table) == ERROR)
	{
		printf("Error join\n");
		return (-1);
	}
	free_data(&table);
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


