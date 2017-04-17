#ifndef EVALUATOR_HH
#define EVALUATOR_HH

/*
 * The class "Evaluator" runs an evaluator in a child process. 
 *
 * When it is opened, Evaluator has four states:
 *
 * property:     Started() Exists() Paused()
 *   - Ready:    0                            no best move known
 *   - Running:  1         1        0         process running
 *   - Paused:   1         1        1         process exists but stopped
 *   - Finished: 1         0                  process terminated and best move known
 *
 * The state can be determined by checking the property above.  
 *
 * Internally these properties correspond to:
 *
 *   STARTED=   started
 *   PROCESS_EXISTS=  IO_Child_Process::Is()
 *   PROCESS_PAUSED=  IO_Child_Process::Paused()
 *
 * State Diagram:
 *    /-----------\                /---------------\
 *    |  Ready    |<-\-reset-------|   Finished    |
 *    \-----------/  |             \---------------/
 *     /|\ |         |                    /|\
 *      | start      |                     |
 *      |  |       reset                   |
 *      |  |         |                     |
 *      |  |      /-------------\          |
 *      |  \----->|  Running    |--stop----/
 *      |         \-------------/          |
 *      |           |      /|\             |
 *      |         pause     |              |
 *      |           |    continue          |
 *    reset         V       |              |
 *      |         /-------------\          |
 *      \---------|  Paused     |--stop----/
 *                \-------------/
 *
 * The combination of RESET + START is called RE_START.
 */

#include "input_reader.hh"
#include "io_child_process.hh"
#include "timer.hh"
#include "position.hh"
#include "generator.hh"
#include "evaluator_settings.hh"
#include "pointer.hh"
#include "engine.hh"
#include "knots.hh"
#include "time.hh"

class Evaluator
	:  protected IO_Child_Process,
	   protected Timer,
	   virtual public Pointed
{
private:

	class EC_Reader
		:  public Input_Reader
	{
	public:
		EC_Reader(Pointer <Evaluator> new_evaluator)
			:  evaluator(new_evaluator)
			{ }
		
	private:

		Pointer <Evaluator> evaluator;
		
		virtual void IR_Input()
			{  evaluator().Input();  }
	};

public:

	Evaluator_Settings settings; 
	Engine engine_choice;

	Evaluator();

	bool Open(const Position &new_position,
		  const Generator &new_generator);
	void Close();

	virtual void EC_Stop()		{ }
	/* When the engine is stopped internally  */

	virtual void EC_Reset()         { }

	virtual void EC_Timer(int /* age_ms     */,
			      int /* cpu_age_ms */)
		/* Called during running of the process and once when
		 * the engine is stopped.   */
		{ }
	virtual void EC_Run() { }
	virtual void EC_Pause() { }
	virtual void EC_Depth(int /* depth */ ) { }
	virtual void EC_Main(int /* move */ )   { }
	virtual void EC_Score(int /* score */ ) { }
	virtual void EC_Knots(knots_t /* knots */ ) { }
	virtual void EC_Depth_Factor() {}

	bool Start();
	void Pause_Continue();
	/* switch between RUNNING and PAUSED */

	bool Paused() const
		{
			assert (Started() && Exists());
			return IO_Child_Process::Paused();
		}
	bool Started() const
		{ return started;  }
	int Best_Move() const
		{
			assert (started); 
			return best_move;
		}
	bool Re_Start();
	/* reset and start */

	struct timeval Start_Time() const
		{ return start_time;  }

//	const IO_Child_Process &Get_IO_Child() const
//		{ return *this;  }

	int Knots() const
		{
			assert (started);
			return knots;
		}

	int Depth() const
		{
			assert (started);
			return depth; 
		}
	int Score() const
		{
			assert (started);
			return score; 
		}
	double Depth_Factor() const
		{
			return depth_factor; 
		}

	const Generator &Get_Generator() const
		{  return generator();   }
	const Position &Get_Position() const
		{ return position(); }
	String Program_Name() const
		{
			return program_name; 
		}
	
	void Input(); 

	int Age_ms() const
		/* Age of the engine process   */
		{
			assert (IO_Child_Process::Is()); 
			if (IO_Child_Process::Paused())
				return age_ms; 
			else
				return Date_Diff_ms(start_time,
						    Now()); 
		}
	int CPU_Age_ms() const
		{  return cpu_age_ms; }

	bool Exists() const
		{
			assert (Started());
			return IO_Child_Process::Is();
		}
	void Reset();
	void Reset_Call()
		{
			Reset();
			EC_Reset(); 
		}
	void Update_CPU_Age();

private:
	enum {milliseconds= 1000};  /* interval */

	bool started;
	int best_move;

	struct timeval start_time; 
	/* when not paused */

	int age_ms;  
	/* in milliseconds, when paused */

	knots_t knots;
	int depth;
	int score;
	int knots_1;
	int knots_n;
	double depth_factor;

	Pointer <const Position> position;
	Pointer <const Generator> generator;
	EC_Reader output_reader, error_output_reader;
	String program_name;
	/* Last run. 
	 */

	int cpu_age_ms; 
	/* CPU running time of the engine.  Updated by Timer_Alarm()
	 * and once when stopped. 
	 */

	pid_t additional_pid;
	/* As reported by the evaluator or <0 */ 

	void Timer_Alarm();
	void IOCP_Output(int i, String);
	void Stop();
	void Stop_Call()
		{
			Stop();
			EC_Stop();
		}
};

#endif /* ! EVALUATOR_HH */
