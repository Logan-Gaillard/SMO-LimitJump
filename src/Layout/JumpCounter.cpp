#include "Layout/JumpCounter.h"
#include "Library/Layout/IUseLayout.h"
#include "Library/Layout/LayoutActor.h"
#include "Library/Layout/LayoutActionKeeper.h"
#include "Library/Layout/LayoutActionFunction.h"
#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Layout/LayoutActionFunction.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"

#include "JumpData.h"

namespace {
    NERVE_IMPL(JumpCounter, Appear)
    NERVE_IMPL(JumpCounter, Wait)
    NERVE_IMPL(JumpCounter, End)
    NERVE_IMPL(JumpCounter, Add)
    NERVE_IMPL(JumpCounter, Sub)
    NERVE_IMPL(JumpCounter, CountAnimChange);

    NERVES_MAKE_NOSTRUCT(JumpCounter, Wait);
    NERVES_MAKE_STRUCT(JumpCounter, Appear, End, Add, Sub, CountAnimChange);
}

JumpCounter::JumpCounter(const char* name, const al::LayoutInitInfo& initInfo) : al::LayoutActor(name){
    al::initLayoutActor(this, initInfo, "JumpCounter", 0);
    mPanelName = "Jump";

    initNerve(&NrvJumpCounter.End, 0);
    kill();
    updatePanel(mJumpRemain);
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
        al::startAction(this, "Appear", nullptr);
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

    if(newJumpRemain > mJumpRemain)
        al::setNerve(this, &NrvJumpCounter.Add);
    else
        al::setNerve(this, &NrvJumpCounter.Sub);

    mJumpRemain = newJumpRemain;
    return true;
}

s32 JumpCounter::getCountFromData() const {
    return JumpData::getJumpRemain();
}



void JumpCounter::exeAppear(){
    if(al::isActionEnd(this, nullptr))
        al::setNerve(this, &Wait);
}

void JumpCounter::exeWait(){
    if(al::isFirstStep(this))
        al::startAction(this, "Wait", nullptr);

    if(mIsUpdateCount)
        tryUpdateCount();
}

void JumpCounter::exeEnd(){
    if(al::isFirstStep(this))
        al::startAction(this, "End", nullptr);

    if(al::isActionEnd(this, nullptr))
        kill();
}

void JumpCounter::exeAdd(){
    if(al::isFirstStep(this)){
        al::startAction(this, "Add", mPanelName);
        updatePanel(mJumpRemain);
    }

    if(al::isActionEnd(this, "Jump"))
        al::setNerve(this, &Wait);
}

void JumpCounter::exeSub(){
    if(al::isFirstStep(this)){
        al::startAction(this, "Sub", mPanelName);
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