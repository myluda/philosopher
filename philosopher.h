#ifndef PHILO_H
# define PHILO_H


# include <pthread.h>
# include <unistd.h>
# include <stdio.h>
# include <sys/time.h>
# include <stdlib.h>

# define FORK 1
# define EAT 2
# define SLEEP 3
# define THINK 4
# define DIE 5

typedef struct s_data
{
	long nbphilo;
	long timeat;
	long timesleep;
	long timetodie;
    unsigned long long	actual_time;
	long nbmeals;
	int  all;
	pthread_mutex_t		*forks;
	pthread_mutex_t     write;
	pthread_mutex_t     eat;
	struct t_philo             *philos;
}t_data;

typedef struct s_philo
{
	pthread_t   thread;
	int id;
	long start;
	long lastime_eat;
	int nb_of_meals;
	int is_eating;
	t_data *datas;
}	t_philo;

int	ft_strlen(char *str);
void    ft_start(t_data *datas, t_philo *nbphilo);
void	init_threads(t_data *datas);
t_philo *ft_init_philo(t_philo *nbphilo, t_data *datas);
unsigned long long	time_pass(unsigned long long	start);
unsigned long long     time_now(void);
void    *work(void *philosophers);
void	ft_think(t_philo *philo);
void	sleepu(unsigned long long end);
void	ft_sleep(t_philo *philo);
void	ft_fork(t_philo *philo);
void	ft_eat(t_philo *philo);
int		ft_print(t_philo *philo, int index , unsigned long long timing);
int	ft_parsing(t_data *args,char **argv,int argc);
int	parse_is_good(char **av);

#endif
