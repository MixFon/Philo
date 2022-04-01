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
	memset(table->philos, 0, sizeof(t_philo) * table->number_of_philosophers);
	table->mutex = (pthread_mutex_t	*)malloc(sizeof(pthread_mutex_t));
	if (table->mutex == NULL)
		return (ERROR);
	if (pthread_mutex_init(table->mutex, NULL) != 0)
		return (ERROR);
	return (SUCCESS);
}


void philo_eat(t_philo *philo)
{
	int curr;
	int next;
	int diff;

	curr = philo->number;
	next = (curr + 1) % philo->table->number_of_philosophers;
	gettimeofday(&philo->end_eat_time, NULL);
	diff = philo->end_eat_time.tv_usec - philo->table->start_time.tv_usec;
	printf("%d %d is eating\n", diff, philo->number + 1);
	usleep(philo->table->time_to_eat);
	philo->table->forks[curr] = 0;
	philo->table->forks[next] = 0;
}

void philo_sleep(t_philo *philo)
{
	struct timeval  time;
	int diff;

	gettimeofday(&time, NULL);
	diff = time.tv_usec - philo->table->start_time.tv_usec;
	printf("%d %d is sleeping\n", diff, philo->number + 1);
	usleep(philo->table->time_to_eat);
}

void philo_thing(t_philo *philo)
{
	struct timeval  time;
	int diff;

	gettimeofday(&time, NULL);
	diff = time.tv_usec - philo->table->start_time.tv_usec;
	printf("%d %d is thinking\n", diff, philo->number + 1);
}

int is_free_forks(t_philo *philo)
{
	int answ;
	int curr;
	int next;

	curr = 0;
	next = 0;
	answ = 0;
	curr = philo->number;
	next = (curr + 1) % philo->table->number_of_philosophers;
	pthread_mutex_lock(philo->table->mutex);
	if (philo->table->forks[curr] == 0 && philo->table->forks[next] == 0)
	{
		philo->table->forks[curr] = 1;
		philo->table->forks[next] = 1;
		answ = 1;
	}
	pthread_mutex_unlock(philo->table->mutex);
	return answ;
}

void *working_philo(void *arg)
{
	t_philo *philo;

	philo = (t_philo *)arg;
	while (21)
	{
		while (21)
		{
			if (is_free_forks(philo))
				break;
		}
		if (philo->is_dead)
		{
			printf("dead %d\n", philo->number);
			break;
		}
		philo_eat(philo);
		philo_thing(philo);
		philo_sleep(philo);
	}
	return (SUCCESS);
}

int start_philo(t_table *table)
{
	int i;
	int status;

	i = -1;
	status = 0;
	gettimeofday(&table->start_time, NULL);
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

	i = -1;
	status = 0;
	while(++i < table->number_of_philosophers)
	{
		status = pthread_join(table->threads[i], NULL);
		if (status != 0)
			return (ERROR);
	}
	return (SUCCESS);
}

void check_dead(t_table *table)
{
	int				i;
	int				diff;
	struct timeval	time;

	i = -1;
	diff = 0;
	while (++i < table->number_of_philosophers)
	{
		if (table->philos[i].is_dead)
			continue;
		gettimeofday(&time, NULL);
		if (table->philos[i].end_eat_time.tv_usec == 0)
			diff = time.tv_usec - table->start_time.tv_usec;
		else
			diff = time.tv_usec - table->philos[i].end_eat_time.tv_usec;
		if (diff > table->time_to_eat)
			table->philos[i].is_dead = 1;
	}
}

int is_all_dead(t_table *table)
{
	int				i;

	i = -1;
	while (++i < table->number_of_philosophers)
	{
		if (table->philos[i].is_dead == 0)
			return (ERROR);
	}
	return (SUCCESS);
}

void *cheking_philo_alive(void *arg)
{
	t_table *table;

	table = (t_table*)arg;
	while(21)
	{
		check_dead(table);
		if (is_all_dead(table))
			break;
	}
	return (SUCCESS);
}

int start_cheking_thread(t_table *table)
{
	if (pthread_create(&table->cheking_thread, NULL, cheking_philo_alive, table))
		return (ERROR);
	return (SUCCESS);
}

int join_cheking_thread(t_table *table)
{
	if (pthread_join(table->cheking_thread, NULL))
		return (ERROR);
	return (SUCCESS);
}

void free_data(t_table *table)
{
	free(table->forks);
	free(table->threads);
	free(table->philos);
	pthread_mutex_destroy(table->mutex);
}

int main(int argc, const char *argv[])
{
    t_table table;

    if (check_args(argc, argv))
    {
        printf("Error arguments\n");
        return (ERROR);
    }
	if (fill_prog(&table, argc, argv) != 0)
	{
		printf("Error malloc\n");
		return (ERROR);
	}
	if (start_philo(&table) == ERROR || start_cheking_thread(&table) == ERROR)
	{
		printf("Error create\n");
		return (ERROR);
	}
	
	if (join_philo(&table) == ERROR || join_cheking_thread(&table) == ERROR)
	{
		printf("Error join\n");
		return (ERROR);
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


