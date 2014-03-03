/** @file TimeQuery.h
*  @brief wrapper for the time_query in openGL
*
*	@author Bartlomiej Filipek 
*	@date May 2012
*/

#include <chrono>

class CpuTimeQuery
{
public:
	double m_time;

protected:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_cpuTimePointStart;

public:
	void begin()
	{
		m_cpuTimePointStart = std::chrono::high_resolution_clock::now();
	}

	void end()
	{
		auto diff = std::chrono::high_resolution_clock::now() - m_cpuTimePointStart;
		auto mili = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		m_time = 0.5*(m_time + (double)mili);
	}
};

/** simple wrapper for the GL_TIME_QUERY from OpenGL */
class GpuTimerQuery
{
public:
    enum class WaitOption 
    { 
        WaitForResults, 
        DoNotWaitForResults 
    };

private:
    GLuint   mQuery;
    GLuint64 mWholeTime;
    GLuint   mCounter;
    double   mTimes[3];
    double   mTime;
public:
    GpuTimerQuery();
    ~GpuTimerQuery();

    /// inits the query, deletes the query if it is created
    void init();

    /// call it before the code you want to measure
    inline void begin();

    /// call it just after the code you want to measure
    inline void end();

    /// call it after end() or at the end of a frame
    void updateResults(WaitOption wait);

    /// resets all the time data (does not delete the query object!)
    inline void resetTime();

    /// @return average time of the whole tests, call it usually at the end of app, in miliseconds
    inline double getAverageTime() const;

    /// @return average time from four tests, updated in updateResults(), in miliseconds 
    inline double &getTime();
private:
    void deleteQuery();
};

///////////////////////////////////////////////////////////////////////////////
// inline:

///////////////////////////////////////////////////////////////////////////////
void GpuTimerQuery::begin()
{
    glBeginQuery(GL_TIME_ELAPSED, mQuery);
}

///////////////////////////////////////////////////////////////////////////////
void GpuTimerQuery::end()
{
    glEndQuery(GL_TIME_ELAPSED);
}

///////////////////////////////////////////////////////////////////////////////
inline void GpuTimerQuery::resetTime() 
{ 
    mWholeTime = 0; 
    mCounter   = 0; 
    mTimes[0] = mTimes[1] = mTimes[2] = 0.0;
    mTime = 0.0;
}

///////////////////////////////////////////////////////////////////////////////
inline double GpuTimerQuery::getAverageTime() const	
{
    double avg = mWholeTime/(double)mCounter;
    avg /= 1000000.0;
    return avg;
}

///////////////////////////////////////////////////////////////////////////////
inline double &GpuTimerQuery::getTime() 
{ 
    return mTime; 
}