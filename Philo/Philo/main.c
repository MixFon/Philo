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

/*
** Количество символов не должно привышать 10
*/
int  check_len_arg(const char *arg)
{
	int i;

	i = 0;
	while(arg[i] != '\0')
		i++;
	return (i >= 11);
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
        if (check_len_arg(argv[i]) != 0)
            return (ERROR);
    }
    return(SUCCESS);
}

int fill_prog(t_table *table, int argc, const char *argv[])
{
	memset(table, 0, sizeof(t_table));
	gettimeofday(&table->start_time, NULL);
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
	memset(table->forks, 0, sizeof(int) * table->number_of_philosophers);
	table->threads = (pthread_t *)malloc(sizeof(pthread_t) * table->number_of_philosophers);
	if (table->threads == NULL)
		return (ERROR);
	table->philos = (t_philo *)malloc(sizeof(t_philo) * table->number_of_philosophers);
	if (table->philos == NULL)
		return (ERROR);
	memset(table->philos, 0, sizeof(t_philo) * table->number_of_philosophers);
	if (pthread_mutex_init(&table->mutex, NULL) != 0)
		return (ERROR);
	return (SUCCESS);
}

long get_diffrent_time(struct timeval *end, struct timeval *start)
{
	struct timeval dtv;

	gettimeofday(end, NULL);
	dtv.tv_sec = end->tv_sec - start->tv_sec;
	dtv.tv_usec = end->tv_usec - start->tv_usec;
	if(dtv.tv_usec < 0)
	{
		dtv.tv_sec--;
		dtv.tv_usec += 1000000;
	}
	return dtv.tv_sec * 1000 + dtv.tv_usec / 1000;
}

void print_time_to_start(t_philo *philo, const char *mess)
{
	struct timeval  time;
	long diff;
	
	diff = get_diffrent_time(&time, &philo->table->start_time);
	printf("%ld %d %s\n", diff, philo->number + 1, mess);
}

void print_forks(t_table *table)
{
	int				i;

	i = -1;
	while (++i < table->number_of_philosophers)
	{
		printf("%d ", table->forks[i]);
	}
	printf("\n");
}

void take_or_put_forks(t_philo *philo, int take)
{
	int curr;
	int next;

	curr = philo->number;
	next = (curr + 1) % philo->table->number_of_philosophers;
	philo->table->forks[curr] = take;
	philo->table->forks[next] = take;
	if (take)
		printf("%d take fork\n", curr + 1);
	else
		printf("%d put fork\n", curr + 1);
	print_forks(philo->table);
}

void philo_eat(t_philo *philo)
{
	long diff;

	pthread_mutex_lock(&philo->table->mutex);
	diff = get_diffrent_time(&philo->end_eat_time, &philo->table->start_time);
	take_or_put_forks(philo, 1);
	philo->is_eat = 1;
	philo->count_eat++;
	print_time_to_start(philo, "is eating");
	pthread_mutex_unlock(&philo->table->mutex);
	usleep(philo->table->time_to_eat * 1000);
}

void philo_sleep(t_philo *philo)
{
	pthread_mutex_lock(&philo->table->mutex);
	take_or_put_forks(philo, 0);
	print_time_to_start(philo, "is sleeping");
	pthread_mutex_unlock(&philo->table->mutex);
	usleep(philo->table->time_to_sleep * 1000);
}

void philo_thing(t_philo *philo)
{
	pthread_mutex_lock(&philo->table->mutex);
	print_time_to_start(philo, "is thinking");
	pthread_mutex_unlock(&philo->table->mutex);
}

int is_free_forks(t_philo *philo)
{
	int answ;
	int curr;
	int next;

	pthread_mutex_lock(&philo->table->mutex);
	answ = 0;
	curr = philo->number;
	next = (curr + 1) % philo->table->number_of_philosophers;
	if (philo->table->forks[curr] == 0 && philo->table->forks[next] == 0)
	{
		print_time_to_start(philo, "has taken a fork");
		//take_or_put_forks(philo, 1);
		answ = 1;
	}
	pthread_mutex_unlock(&philo->table->mutex);
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
			if (is_free_forks(philo) == 1)
				break;
		}
		if (philo->is_dead)
			break;
		philo_eat(philo);
		if (philo->is_dead)
			break;
		philo_sleep(philo);
		if (philo->is_dead)
			break;
		philo_thing(philo);
		if (philo->is_dead)
			break;
	}
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

	i = -1;
	while(++i < table->number_of_philosophers)
		pthread_join(table->threads[i], NULL);
	return (SUCCESS);
}

void check_dead(t_table *table)
{
	int				i;
	long			diff;
	struct timeval	time;

	i = -1;
	while (++i < table->number_of_philosophers)
	{
		if (table->philos[i].is_dead == 1)
			continue;
		if (table->philos[i].is_eat == 1)
			diff = get_diffrent_time(&time, &table->philos[i].end_eat_time);
		else
			diff = get_diffrent_time(&time, &table->start_time);
			
		if (diff > table->time_to_die
			|| (table->philos[i].count_eat >= table->number_of_eat
			&& table->number_of_eat != 0))
		{
			table->philos[i].is_dead = 1;
			diff = get_diffrent_time(&time, &table->start_time);
			pthread_detach(table->threads[i]);
			printf("%ld %d died\n", diff, i + 1);
		}
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

void print_dead(t_table *table)
{
	int				i;

	i = -1;
	while (++i < table->number_of_philosophers)
	{
		printf("%d ", table->philos[i].is_dead);
	}
	printf("\n");
}


void *cheking_philo_alive(void *arg)
{
	t_table *table;

	table = (t_table*)arg;
	while(21)
	{
		//printf("Hello\n");
		check_dead(table);
		//print_dead(table);
		if (is_all_dead(table) == SUCCESS)
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
	pthread_mutex_destroy(&table->mutex);
}

int print_error_message(const char *mess)
{
	printf("%s\n", mess);
	return (ERROR);
}

int check_numbers_args(t_table *table)
{
	if (table->time_to_eat <= 60)
		return (ERROR);
	if (table->time_to_die <= 60)
		return (ERROR);
	if (table->time_to_sleep <= 60)
		return (ERROR);
	if (table->number_of_philosophers > 200)
		return (ERROR);
	return (SUCCESS);
}

int main(int argc, const char *argv[])
{
    t_table table;

    if (check_args(argc, argv))
        return (print_error_message("Error arguments"));
	if (fill_prog(&table, argc, argv) != 0)
        return (print_error_message("Error malloc"));
	if (check_numbers_args(&table) == ERROR)
        return (print_error_message("Error create"));
	if (start_philo(&table) == ERROR)
        return (print_error_message("Error create"));
	cheking_philo_alive(&table);
	join_philo(&table);
	free_data(&table);
    return 0;
}
