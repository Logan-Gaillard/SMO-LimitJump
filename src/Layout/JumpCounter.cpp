#include "Layout/JumpCounter.h"
#include "Library/Layout/IUseLayout.h"
#include "Library/Layout/LayoutActor.h"
#include "Library/Layout/LayoutActionKeeper.h"
#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Layout/LayoutActionFunction.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"

#include "logger/SDLogger.hpp"

#include "JumpData.h"

namespace {
    NERVE_IMPL(JumpCounter, Appear)
    NERVE_IMPL(JumpCounter, Wait)
    NERVE_IMPL(JumpCounter, End)
    NERVE_IMPL(JumpCounter, Change)
    NERVE_IMPL(JumpCounter, CountAnimChange)

    NERVES_MAKE_NOSTRUCT(JumpCounter, Wait);
    NERVES_MAKE_STRUCT(JumpCounter, Appear, End, Change, CountAnimChange);
}

JumpCounter::JumpCounter(const char* name, const al::LayoutInitInfo& initInfo) : al::LayoutActor(name){
    al::initLayoutActor(this, initInfo, "JumpCounter", 0);
    mNumDigits = 2;
    mPanelName = "Jump";

    initNerve(&NrvJumpCounter.End, 0);
    kill();
    updatePanel(mJumpRemain);

    SDLogger::log("JumpCounter main group : %s", this->mLayoutActionKeeper->mMainGroupName);
}

void JumpCounter::kill(){
    al::setNerve(this, &NrvJumpCounter.End);
    al::LayoutActor::kill();
}

void JumpCounter::updatePanel(u32 jumpRemain) {
    al::IUseLayout* layout = static_cast<al::LayoutActor*>(this);
    al::setPaneStringFormat(layout, "TxtCounter", "%02d", jumpRemain);
    al::setPaneStringFormat(layout, "TxtCounterSh", "%02d", jumpRemain);
    al::requestCaptureRecursive(this);
}

bool JumpCounter::isWait() const {
    return al::isNerve(this, &Wait);
}


void JumpCounter::tryStart(){
    if(al::isNerve(this, &NrvJumpCounter.End)){
        al::startAction(this, "Appear", "Main");
        updateCountImmidiate();
        al::LayoutActor::appear();
        al::setNerve(this, &NrvJumpCounter.Appear);
    }
}

void JumpCounter::updateCountImmidiate(){
    s32 newJumpCount = getCountFromData();

    mJumpRemain = newJumpCount;
    updatePanel(mJumpRemain);
}

void JumpCounter::tryEnd(){
    if(!al::isNerve(this, &NrvJumpCounter.End)){
        al::setNerve(this, &NrvJumpCounter.End);
    }
}

void JumpCounter::startCountAnim(s32 jumpNum){
    mPrevJumpRemain = mJumpRemain;
    mJumpRemain = jumpNum;

    al::setNerve(this, &NrvJumpCounter.CountAnimChange);

}

bool JumpCounter::tryUpdateCount(){
    s32 newJumpRemain = getCountFromData();

    if(newJumpRemain == mJumpRemain){
        return false;
    }

    mJumpRemain = newJumpRemain;
    al::setNerve(this, &NrvJumpCounter.Change);
    return true;
}

s32 JumpCounter::getCountFromData() const {
    return JumpData::getJumpRemain();
}



void JumpCounter::exeAppear(){
    if(al::isActionEnd(this, "Main"))
        al::setNerve(this, &Wait);
}

void JumpCounter::exeWait(){
    if(al::isFirstStep(this))
        al::startAction(this, "Wait", "Main");

    if(mIsUpdateCount)
        tryUpdateCount();
}

void JumpCounter::exeEnd(){
    if(al::isFirstStep(this))
        al::startAction(this, "End", "Main");

    if(al::isActionEnd(this, "Main"))
        kill();
}

void JumpCounter::exeChange(){
    if(al::isFirstStep(this)){
        al::startAction(this, "Change", mPanelName);
        updatePanel(mJumpRemain);
    }

    if(al::isActionEnd(this, "Jump"))
        al::setNerve(this, &Wait);
}

void JumpCounter::exeCountAnimChange(){
    if(al::isFirstStep(this))
        mAnimationCount = mPrevJumpRemain;

    if(al::isGreaterEqualStep(this, 60)){
        updatePanel(mJumpRemain);
        al::setNerve(this, &Wait);
        return;
    }

    s32 animationCount = al::calcNerveValue(this, 60, mPrevJumpRemain, mJumpRemain);
    updatePanel(animationCount);
}