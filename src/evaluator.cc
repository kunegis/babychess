#include "evaluator.hh"

#include "fen.hh"
#include "parse_move.hh"
#include "parser.hh"
#include "error.hh"
#include "install_dir.hh"
#include "filename.hh"
#include "score.hh"

#include <math.h>
#include <sys/resource.h>

#include <algorithm>

bool Evaluator::Start()
{
	assert (! IO_Child_Process::Is());

	if (started)  return true;

	String fen= FEN(position());

	const char *args[]= {NULL, "-p", fen(), NULL, NULL};

	switch (engine_choice.type)
	{
	default:
	case et_babychess:
		program_name = "babychess-evaluator"; 
		break;

	case et_xboard:
		program_name = "babychess-xboard-evaluator";
		args[3]= engine_choice.program();
		break;
	}
	program_name= Install_Dir(id_bin) / program_name; 

	if (! IO_Child_Process::Open(program_name(), 
				     args,
				     !(settings.flags & evf_no_nice)))
		return false;

	started= true;
  
	EC_Run();

	output_reader.Open(fileno(IO_Child_Process::Output()));
	error_output_reader.Open(fileno(IO_Child_Process::Error_Output()));

	best_move= *generator() ? 0 : -1;
	Timer::Start(milliseconds);
	start_time= Now();
	knots= 0;
	depth= 0;
	score= 0;
	knots_1= 1;
	cpu_age_ms= 0;
	depth_factor= 2;
	additional_pid= -1;

	return true;
}

void Evaluator::Pause_Continue()
{
	if (! IO_Child_Process::Is())  return;

	IO_Child_Process::Pause(true);
	/* This uses SIGUSR1 instead of SIGSTOP, which makes the child
	 * process also stop it's children.  */

	if (IO_Child_Process::Paused())
	{
		age_ms= Date_Diff_ms(start_time, Now());
		if (Timer::Is()) Timer::Stop();
	}
	else
	{
		start_time= Now() + - age_ms;
		Timer::Start(milliseconds);
	}
	EC_Pause(); 
}

void Evaluator::IOCP_Output(int, String line)
{
	Parser parser(line); 

	parser.Skip_Space(); 

	String word= parser.Skip_Word(); 
	parser.Skip_Space();

	if (word == "depth")
	{
		String value= parser.Skip_Word(); 
		if (! value.Is())  return;
		int new_depth;
		if (1 != sscanf(value(), "%d", & new_depth))  return;
		if (new_depth <= depth)  return;
		depth= new_depth;
		knots_n= -1;
		EC_Depth(depth);
	}
	else if (word == "main")
	{
		String move= parser.Skip_Word(); 
		int new_move= Parse_Move(move(), generator());
		if (new_move >= 0)
		{
			best_move= new_move;
			EC_Main(new_move);
		}
	}
	else if (word == "score")
	{
		String new_score_string= parser.Skip_Word(); 

		score= Parse_Score(new_score_string);

		EC_Score(score);
	}
	else if (word == "knots")
	{
		String value= parser.Skip_Word();

		knots_t new_knots;
		if (1 == sscanf(value(), "%"SCNknots, & new_knots))
		{
			knots= new_knots;
			if (depth == 1 && knots_1 == 1)
				knots_1= std::max(knots, (knots_t) 1); 
			else if (depth > 1 && knots_n < 0)
			{
				knots_n= 1;
				
				depth_factor= pow((double)(knots / (knots_t)knots_1),
						  1.0 / ((double)(depth-1)));
				EC_Depth_Factor();
			}
		}

		EC_Knots(knots);
	}
	else if (word == "pid")
	{
		unsigned pid;
		if (parser.Next_Format("%u", &pid))
			additional_pid= pid; 
	}
}

bool Evaluator::Re_Start()
{
	Reset(); 
	return Start();
}

void Evaluator::Timer_Alarm()
{
	struct timeval now= Now();
	int date_ms= Date_Diff_ms(start_time, now);

	Update_CPU_Age();
	
	EC_Timer(date_ms, cpu_age_ms);


	/* Terminate if necessary */
	if (settings.timing == ti_time   &&
	    date_ms >= 1000 * settings.time_s)
	{
		Stop_Call();
	}
}

void Evaluator::Reset()
{
	if (! Started())   return;

	if (Exists())
	{
		output_reader.Close();
		error_output_reader.Close();

		if (Timer::Is())
			Timer::Stop();
		IO_Child_Process::Close();
		started= false;
	}

	else /* finished */ 
	{
		started= false;
	}
}

void Evaluator::Input()
{
	assert (IO_Child_Process::Is());
	if (Proceed())
	{
		Stop_Call();
	}
}

Evaluator::Evaluator()
	:  output_reader(this),
	   error_output_reader(this)
{
}

bool Evaluator::Open(const Position &new_position,
		     const Generator &new_generator)
{
	position= new_position;
	generator= new_generator;
	settings.Reset(); 
	started= false;
			
	return true;
}

void Evaluator::Close()
{
	assert (!Started());
	position= NULL;
	generator= NULL;
}

void Evaluator::Stop()
{
	if (Timer::Is())
		Timer::Stop();
	output_reader.Close();
	error_output_reader.Close();
	struct rusage rusage;
	IO_Child_Process::Close(&rusage); 
	struct timeval now= Now();
	int date_ms= Date_Diff_ms(start_time, now);
	cpu_age_ms= std::max((long)cpu_age_ms, 
//	cpu_age_ms >?= 
		1000 * (rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec)
			     + (rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000);
	EC_Timer(date_ms, cpu_age_ms);
}

void Evaluator::Update_CPU_Age()
{
//	Process_Usage usage= IO_Child_Process::Get_Process_Usage();
//	int time_ms= usage.total + usage.children_total; 
	int time_ms= IO_Child_Process::Usage();
	if (additional_pid >= 0)
	{
//		Process_Usage additional_usage=
//			::Get_Process_Usage(additional_pid);
		time_ms += //(additional_usage.total +
//			    additional_usage.children_total);
			Process_Usage(additional_pid);
	}

	cpu_age_ms= std::max(cpu_age_ms, time_ms); 
//	cpu_age_ms >?= time_ms;
}
