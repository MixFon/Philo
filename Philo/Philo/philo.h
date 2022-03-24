//
//  philo.h
//  Philo
//
//  Created by Михаил Фокин on 22.03.2022.
//

#ifndef philo_h
#define philo_h

#include "stdio.h"
#include "string.h"
#include "sys/time.h"
#include "pthread.h"

typedef struct      s_prog
{
    struct timeval  start_time;
    int             number_of_philosophers;
    int             time_to_die;
    int             time_to_eat;
    int             time_to_sleep;
	int 			number_of_eat;
    int             *forks;
}                   t_table;

typedef struct      s_philo
{
    int             number;
    t_table         *table;
    struct timeval  woke_up;
}                   t_philo;

int    ft_atoi(const char *str);

#endif
