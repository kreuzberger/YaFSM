#ifndef SIMPLESCXMLFSMDATAMODEL_H
#define SIMPLESCXMLFSMDATAMODEL_H

#include <iostream>

class SimpleScxmlFSMDataModel
{

public:
  void onEnterRun( void )   {  std::cout << "onEnterRun called on 'TestSimpleScxmlFSM'" << std::endl;  }
  void onExitRun( void )    {  std::cout << "onExitRun called on 'TestSimpleScxmlFSM'" << std::endl;  }
  //virtual void onRun( void )        {  std::cout << "onRun called on 'TestSimpleFSM'" << std::endl;  }
  void onRun( void )        {  std::cout << "onRun called on 'TestSimpleScxmlFSM'" << std::endl; }

};


#endif // SIMPLESCXMLFSMDATAMODEL_H
