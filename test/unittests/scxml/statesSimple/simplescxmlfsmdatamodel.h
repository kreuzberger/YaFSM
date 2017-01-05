#ifndef SIMPLESCXMLFSMDATAMODEL_H
#define SIMPLESCXMLFSMDATAMODEL_H

#include <iostream>

class run
{

public:
  std::string name;
  std::string data;
};

class end
{
public:
  int i;

};

class SimpleScxmlFSMDataModel
{
public:
  SimpleScxmlFSMDataModel()
  : mCompareState(0)
  {}

public:
  void onEnterRunning( void )   {  std::cout << "onEnterRun called on 'TestSimpleScxmlFSM'" << std::endl; mCompareState = 2; }
  void onExitRun( void )    {  std::cout << "onExitRun called on 'TestSimpleScxmlFSM'" << std::endl;  mCompareState = 3;}
  //virtual void onRun( void )        {  std::cout << "onRun called on 'TestSimpleFSM'" << std::endl;  }
  void onRun( void )        {  std::cout << "onRun called on 'TestSimpleScxmlFSM'" << std::endl; mCompareState = 1;}

private:
  int mCompareState;

};


#endif // SIMPLESCXMLFSMDATAMODEL_H
