/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayajrhou <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/23 07:55:11 by ayajrhou          #+#    #+#             */
/*   Updated: 2021/10/23 07:55:13 by ayajrhou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "philo.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

int	ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}
int	parse_is_good(char **av)
{
	int	i;
	int	j;

	i = 1;
	j = 0;
	while (av[i])
	{
		j = 0;
		while (av[i][j])
		{
			if (av[i][j] > '9' || av[i][j] < '0' || ft_strlen(av[i]) > 10)
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}

int	ft_parsing(t_data *args,char **argv,int argc)
{
	if (parse_is_good(argv))
	{
		args->all = 0;
		args->nbphilo = atoi(argv[1]);
		if(args->nbphilo == 0)
			return (0);
		args->timetodie = atoi(argv[2]);
		args->timeat = atoi(argv[3]);
		args->timesleep = atoi(argv[4]);
		if (argc == 6)
			args->nbmeals = atoi(argv[5]);
		else
			args->nbmeals = -1;
	}
	return parse_is_good(argv);
}
int		ft_print(t_philo *philo, int index , unsigned long long timing)
{
	if (pthread_mutex_lock(&philo->datas->write))
		return (1);
	if (index == EAT)
		printf("%llu %d is eating",timing,philo->id + 1);
	else if (index == THINK)
		printf("%llu %d is thinking",timing, philo->id + 1);
	else if (index == FORK)
		printf("%llu %d grabbing a fork", timing , philo->id + 1);
	else if (index == SLEEP)
		printf("%llu %d is sleeping",timing, philo->id + 1);
	else if (index == DIE)
		printf("%llu %d is dead",timing,philo->id +1 );
	pthread_mutex_unlock(&philo->datas->write);
		return (1);
	return (0);

}
void	ft_eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->datas->eat);
	philo->lastime_eat = time_now();
	pthread_mutex_unlock(&philo->datas->eat);
	philo->is_eating = 1;
	ft_print(philo,EAT,time_pass(philo->start));
	usleep((philo->datas->timeat * 1000) - 10000);
	sleepu(philo->lastime_eat + philo->datas->timeat);
	pthread_mutex_lock(&philo->datas->eat);
	philo->is_eating = 0;
	if (philo->nb_of_meals == philo->datas->nbmeals)
		philo->datas->all++;
	pthread_mutex_lock(&philo->datas->eat);

}
void	ft_fork(t_philo *philo)
{
	int right;
	int left;

	if (philo->id % 2 == 0)
		right = (philo->id + 1) % (philo->datas->nbphilo);
	else
		right = philo->id;
	if (philo->id % 2 == 0)
		left = philo->id;
	else
		left = (philo->id + 1) % (philo->datas->nbphilo);
	pthread_mutex_lock(&philo->datas->forks[left]);
	ft_print(philo,FORK,time_pass(philo->start));
	pthread_mutex_lock(&philo->datas->forks[right]);
	ft_print(philo,FORK,time_pass(philo->start));
	ft_eat(philo);
	pthread_mutex_unlock(&philo->datas->forks[left]);
	pthread_mutex_unlock(&philo->datas->forks[right]);	
}

void	ft_sleep(t_philo *philo)
{
	unsigned long long	before_sleep;

	before_sleep = time_now();
	ft_print(philo, SLEEP, time_pass(philo->start));
	usleep((philo->datas->timesleep * 1000) - 10000);
	sleepu(philo->datas->timesleep + before_sleep);
}

void	sleepu(unsigned long long end)
{
	while (time_now() < end)
		usleep(10);
}

void	ft_think(t_philo *philo)
{
	ft_print(philo, THINK, time_pass(philo->start));
}
void    *work(void *philosophers)
{
    t_philo *philo;
    philo = (t_philo *)philosophers;

	while(1)
	{
		ft_fork(philo);
		ft_think(philo);
		ft_sleep(philo);
	}
}

unsigned long long     time_now(void)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	return ((now.tv_sec * 1000) + (now.tv_usec / 1000));
}

unsigned long long	time_pass(unsigned long long	start)
{
	struct timeval	end;

	gettimeofday(&end, NULL);
	return (((end.tv_sec * 1000) + (end.tv_usec / 1000)) - start);
}
t_philo *ft_init_philo(t_philo *nbphilo, t_data *datas)
{
	int i;

	i = 0;
    nbphilo = malloc(sizeof(t_philo) * datas->nbphilo);
	if (!nbphilo)
		return (NULL);
	while(i++ < datas->nbphilo)
	{
        nbphilo[i].datas = datas;
		nbphilo[i].id = i;
        nbphilo[i].is_eating = 0;
        nbphilo[i].nb_of_meals = 0;
        nbphilo[i].lastime_eat = time_now();
        nbphilo[i].start = nbphilo->lastime_eat;
	}
}

void	init_threads(t_data *datas)
{
	int i;

	i = 0;
    datas->forks = malloc((datas->nbphilo) * sizeof(pthread_mutex_t));
	while(i++ < datas->nbphilo)
        pthread_mutex_init(&datas->forks[i],NULL);
    pthread_mutex_init(&datas->write,NULL);
    pthread_mutex_init(&datas->eat,NULL);
}

void    ft_start(t_data *datas, t_philo *nbphilo)
{
    int i;

    nbphilo = ft_init_philo(nbphilo,datas);
    init_threads(datas);
	datas->actual_time = time_now();
	while (i < datas->nbphilo)
	{
		if (pthread_create(&nbphilo[i].thread, NULL, work, &nbphilo[i]))
			printf("error\n");
		usleep(100);
		i++;
	}

}

int main(int argc , char **argv)
{
	t_data *datas;
	t_philo *nbphilo;

	datas = NULL;
	if (argc == 5 || argc == 6)
	{
		if	(!ft_parsing(datas,argv,argc))
			exit(0);
		ft_start(datas,nbphilo);
	}
		
}
