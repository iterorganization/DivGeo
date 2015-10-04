#include "MainWnd.h"

/*void CMainWnd::SetHighlightMode( bool enable )
{
  if( enable == highlightMode )
    return;

  DrawAppHighlight( DRAW_OFF );
  highlightMode = enable;
  DrawAppHighlight( DRAW_ON );
}

void CMainWnd::DrawAppObject( IComponentPtr pObject, int mode )
{
  ValidatePtr( pObject, "DrawAppObject_" );
  foreach( QMdiSubWindow* pSubWnd, pMdiArea->subWindowList() ) {
     CViewWnd* pSubView = qobject_cast< CViewWnd* >( pSubWnd->widget() );
     pSubView->DrawObject( pObject, mode );
  }
}

void CMainWnd::DrawAppHighlight( int mode )
{
  switch( mode ) {
    case DRAW_ON:
     assert( highlightLocks != 0 );
     if( highlightLocks < 0 )
       highlightLocks++;
     else {
       highlightLocks--;
       if( highlightLocks == 0 ) {
         pModel->Highlighted_Reset();
         while( !pModel->Highlighted_End() ) {
           IComponentPtr pObject = pModel->Highlighted_Next();
           DrawAppObject( pObject, mode );
         }
       }
     }

     foreach( QMdiSubWindow* pSubWnd, pMdiArea->subWindowList() ) {
       CViewWnd* pSubView = qobject_cast< CViewWnd* >( pSubWnd->widget() );
       pSubView->DrawHighlightRect( DRAW_ON );
       pSubView->DrawAllShapes( DRAW_ON );
     }
     break;

  case DRAWHI_UNLOCK:
    assert( highlightLocks != 0 );
    highlightLocks--;
    break;

  case DRAWHI_LOCK:
    if( highlightLocks < 0 )
      DrawAppHighlight( DRAW_OFF );
    else
      highlightLocks++;
    break;

  case DRAWHI_CNDOFF:
    if( highlightLocks <= 0 )
      highlightLocks--;
    else
      highlightLocks++;
    break;

  case DRAW_OFF:
    if( highlightLocks <= 0 ) {
      foreach( QMdiSubWindow* pSubWnd, pMdiArea->subWindowList() ) {
        CViewWnd* pSubView = qobject_cast< CViewWnd* >( pSubWnd->widget() );
        pSubView->DrawHighlightRect( DRAW_OFF );
        pSubView->DrawAllShapes( DRAW_OFF );
      }
      pModel->Highlighted_Reset();
      while( !pModel->Highlighted_End() ) {
        IComponentPtr pObject = pModel->Highlighted_Next();
        DrawAppObject( pObject, mode );
      }
      highlightLocks = -highlightLocks;
    }
    highlightLocks++;
    break;

  default:
    assert( false );
  }
}

void CMainWnd::AddAppUpdate()
{
  if( pModel != null && pModel->UpdateLocks() != 0 )
    return;

  foreach( QMdiSubWindow* pSubWnd, pMdiArea->subWindowList() ) {
    CViewWnd* pSubView = qobject_cast< CViewWnd* >( pSubWnd->widget() );
    pSubView->SetUpdateFlag( UPDATE_MAX );
  }
}*/
