#include "simple_poll.hh"

#include "input_reader.hh"
#include "default_timer.hh"
#include "time.hh"

fd_set simple_poll_fd_set;

int Register_Input(Input_Reader &ir)
{
	FD_SET(ir.FD(), & simple_poll_fd_set);
	return 0; 
}

void Unregister_Input(Input_Reader &ir)
{
	FD_CLR(ir.FD(), & simple_poll_fd_set);
}

void Simple_Poll()
{
	fd_set ready_fd_set= simple_poll_fd_set;

	int timer_index= -1;
	struct timeval earliest= {-1, 0};
	struct timeval now_date= Now();
	
	for (int i= 0;  i < *timer_list;  ++i)
	{
		if (timer_list[i].Is())
		{
			struct timeval date= timer_list[i]().content().last_alarm;
			date += timer_list[i]().content().interval;
			if (Date_Diff_ms(now_date, date) < 0)
			{
				timer_list[i]().content().last_alarm= Now();
				timer_list[i]().Timer_Alarm();
				date= now_date;
				date += timer_list[i]().content().interval; 
			}
			if ((! earliest.tv_sec>=0) || (Date_Diff_ms(date, earliest) >= 0))
			{
				timer_index= i;
				earliest= date;
			}
		}
	}

	struct timeval *poll_time= NULL;
	struct timeval time;

	if (earliest.tv_sec >= 0)
	{
		int ms= Date_Diff_ms(Now(), earliest);
		time= ms_To_Time(ms);
		poll_time= & time; 
	}

	if (0 > select(FD_SETSIZE, & ready_fd_set, NULL, NULL, poll_time))
		return;

	if (poll_time != NULL)
	{
		if (Date_Diff_ms(earliest, Now()) >= 0)
		{
			timer_list[timer_index]().content().last_alarm= Now();
			timer_list[timer_index]().Timer_Alarm();
		}
	}

	for (int i= 0;  i < *Input_Reader::Get_List();  ++i)
	{
		if (Input_Reader::Get_List()[i].Is() &&
		    FD_ISSET(Input_Reader::Get_List()[i]().FD(), &ready_fd_set))
			Input_Reader::Get_List()[i]().IR_Input(); 
	}
}
