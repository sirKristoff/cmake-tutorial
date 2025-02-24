/**
 * @file lifetime_tracker.cpp
 *
 * @remark This code is inspired by the materials included with the book:
 *   Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
 *   Patterns Applied". Copyright (c) 2001. Addison-Wesley.
 */

#include "singleton/singleton.hpp"

#include <algorithm>
#include <new>


using namespace lifetime_policy::internal;

// Helper data

namespace
{
/**  LifetimeTracker objects collection.  */
typedef  LifetimeTracker**  TrackerArray;
/**
 * Priority stack of LifetimeTracker objects (higher longevity closer to the top)
 */
TrackerArray  pTrackerArray = nullptr;
/**  Size of pTrackerArray  */
unsigned int  elements = 0u;
}


////////////////////////////////////////////////////////////////////////////////
// method  LifetimeTracker::LifetimeTracker
////////////////////////////////////////////////////////////////////////////////

LifetimeTracker::LifetimeTracker(unsigned int longevity)
: longevity_(longevity)
{}


////////////////////////////////////////////////////////////////////////////////
// method  LifetimeTracker::~LifetimeTracker
////////////////////////////////////////////////////////////////////////////////

LifetimeTracker::~LifetimeTracker()
{}


////////////////////////////////////////////////////////////////////////////////
// method  LifetimeTracker::compare
////////////////////////////////////////////////////////////////////////////////

bool LifetimeTracker::compare(
    const LifetimeTracker* lhs,
    const LifetimeTracker* rhs)
{
    return lhs->longevity_ < rhs->longevity_;
}


////////////////////////////////////////////////////////////////////////////////
// method  LifetimeTracker::registerTracker
////////////////////////////////////////////////////////////////////////////////

void  LifetimeTracker::registerTracker(LifetimeTracker* newTracker)
{
    if (!newTracker)
        return;

    TrackerArray pNewArray = static_cast<TrackerArray>(
            std::realloc(pTrackerArray,
                sizeof(*pTrackerArray) * (elements + 1)));
    if (!pNewArray)
        throw std::bad_alloc();

    // Delayed assignment for exception safety
    pTrackerArray = pNewArray;

    // Insert a pointer to the object into the queue

    /** @brief Position in pTrackerArray behind last element
     *  with longevity equal @c newTracker->longevity_  */
    TrackerArray lastTrackerWithLongevityItr = std::upper_bound(
        pTrackerArray,
        pTrackerArray + elements,
        newTracker,
        LifetimeTracker::compare);
    /* Move to back Tracker array elements
     * from position of lastTrackerWithLongevityItr,
     * starts from last element in copy range (pTrackerArray + elements).
     */
    std::copy_backward(
        lastTrackerWithLongevityItr,
        pTrackerArray + elements,
        pTrackerArray + elements + 1);
    *lastTrackerWithLongevityItr = newTracker;  // new Tracker into inner position
    ++elements;
}


////////////////////////////////////////////////////////////////////////////////
// method  LifetimeTracker::atexit_fun
////////////////////////////////////////////////////////////////////////////////

void  LifetimeTracker::atexit_fun()
{
    assert(0 < elements && 0 != pTrackerArray);
    // Pick the element at the top of the stack
    LifetimeTracker* pTop = pTrackerArray[elements - 1];
    // Remove that object off the stack
    // Don't check errors - realloc with less memory 
    //     can't fail
    pTrackerArray =
        static_cast<TrackerArray>(
            std::realloc(
                pTrackerArray,
                sizeof(*pTrackerArray) * --elements));
    // Destroy the element
    delete pTop;
}
