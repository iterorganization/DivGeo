#include "ResourceTree.h"

const QStringList TreeNode::empty = QStringList();

TreeNode* TreeNode::Node( const QString& path )
{
  if( path.trimmed().isEmpty() )
    return null;
  int dot = path.indexOf( '/' );
  if( dot == -1 ) {
    dot = path.indexOf( '.' );
    if( dot == -1 )
      return nodes.value( path.trimmed(), null );
  }
  TreeNode* pNode = nodes.value( path.left( dot ).trimmed(), null );
  if( pNode != null )
    return pNode->Node( path.mid( dot+1 ).trimmed() );
  else
    return null;
}

ulong TreeNode::Code( const QString& path, bool mark )
{
  QString trimmed = path.trimmed();
  if( trimmed.isEmpty() )
    return DataValue::EMPTY;
  int dot = path.indexOf( '/' );
  if( dot == -1 ) {
    dot = path.indexOf( '.' );
    if( dot == -1 ) {
      DataValue* pValue = values.value( trimmed, null );
      ulong code = DataValue::EMPTY;
      if( mark && pValue != null ) {
        pValue->mark = true;
        unmarked_ready = false;
        code = pValue->code;
      }
      return code;
    }
  }

  TreeNode* pNode = nodes.value( path.left( dot ).trimmed(), null );
  if( pNode != null ) {
    ulong code = pNode->Code( path.mid( dot+1 ).trimmed(), mark );
    if( mark && code != DataValue::EMPTY )
      unmarked_ready = false;
    return code;
  }
  else
    return DataValue::EMPTY;
}

const QStringList& TreeNode::Data( const QString& path ) const
{
  QString trimmed = path.trimmed();
  if( trimmed.isEmpty() )
    return empty;
  int dot = path.indexOf( '/' );
  if( dot == -1 ) {
    dot = path.indexOf( '.' );
    if( dot == -1 ) {
      DataValue* pValue = values.value( path, null );
      return (pValue == null) ? empty : pValue->strings;
    }
  }

  TreeNode* pNode = nodes.value( path.left( dot ).trimmed(), null );
  if( pNode != null )
    return pNode->Data( path.mid( dot+1 ).trimmed() );
  else
    return empty;
}

DataValue* TreeNode::Value( const QString& path ) const
{
  QString trimmed = path.trimmed();
  if( trimmed.isEmpty() )
    return null;
  int dot = path.indexOf( '/' );
  if( dot == -1 ) {
    dot = path.indexOf( '.' );
    if( dot == -1 )
      return values.value( path, null );
  }

  TreeNode* pNode = nodes.value( path.left( dot ).trimmed(), null );
  return (pNode != null) ? pNode->Value( path.mid( dot+1 ).trimmed() ) : null;
}

TreeNode* TreeNode::AddNode( const QString& _path )
{
  QString path_tr = _path.trimmed();
  if( path_tr.isEmpty() )
    return null;
  int dot = path_tr.indexOf( '/' );
  if( dot == -1 ) {
    dot = path_tr.indexOf( '.' );
    if( dot == -1 ) {
      TreeNode* pNode = nodes.value( path_tr, null );
      if( pNode == null ) {
        nodes_ordered.push_back( TreeNode( path + path_tr ) );
        pNode = &nodes_ordered.last();
        nodes.insert( path_tr, &nodes_ordered.last() );
      }
      return pNode;
    }
  }

  QString root = _path.left( dot ).trimmed();
  TreeNode* pNode = nodes.value( root, null );
  if( pNode == null ) {
    nodes_ordered.push_back( TreeNode( path + root ) );
    pNode = &nodes_ordered.last();
    nodes.insert( root, pNode );
  }
  return pNode->AddNode( _path.mid( dot+1 ).trimmed() );
}

TreeNode* TreeNode::AddValue(const QString& _path, ulong _code )
{
  if( _path.trimmed().isEmpty() )
    return null;
  int dot = _path.indexOf( '/' );
  if( dot == -1 ) {
    dot = _path.indexOf( '.' );
    if( dot == -1 ) {
      if( values.value( _path.trimmed(), null ) == null ) {
        values_ordered.push_back( DataValue( _path, _code ) );
        values.insert( _path, &values_ordered.last() );
        unmarked_ready = false;
        return this;
      }
      else
        return null;
    }
  }

  QString root = _path.left( dot ).trimmed();
  TreeNode* pNode = nodes.value( root, null );
  if( pNode == null ) {
    nodes_ordered.push_back( TreeNode( path + root ) );
    pNode = &nodes_ordered.last();
    nodes.insert( root, &nodes_ordered.last() );
  }
  TreeNode* pResult = pNode->AddValue( _path.mid( dot+1 ), _code );
  if( pResult != null )
    unmarked_ready = false;
  return pResult;
}

TreeNode* TreeNode::SetValue( const QString& _path, const QStringList& _crStrings, bool _append )
{
  if( _path.trimmed().isEmpty() )
    return null;
  int dot = _path.indexOf( '/' );
  if( dot == -1 ) {
    dot = _path.indexOf( '.' );
    if( dot == -1 ) {
      DataValue* pValue = values.value( _path, null );
      if( pValue != null ) {
        QStringList& rData = pValue->strings;
        if( !rData.isEmpty() && _append ) {
          rData.last().append( _crStrings.first() );
          foreach( const QString& crStr, _crStrings )
            if( &crStr != &_crStrings.first() )
              rData.append( crStr );
        }
        else {
          rData.clear();
          rData.append( _crStrings );
        }
      }
      else {
        values_ordered.push_back( DataValue( _path, _crStrings ) );
        values.insert( _path, &values_ordered.last() );
      }

      unmarked_ready = false;
      return this;
    }
  }

  QString root = _path.left( dot ).trimmed();
  TreeNode* pNode = nodes.value( root, null );
  if( pNode == null ) {
    nodes_ordered.push_back( TreeNode( path + root ) );
    pNode = &nodes_ordered.last();
    nodes.insert( root, &nodes_ordered.last() );
  }
  TreeNode* pResult = pNode->SetValue( _path.mid( dot+1 ), _crStrings );
  if( pResult != null )
    unmarked_ready = false;
  return pResult;
}

const QStringList& TreeNode::GetUnmarked()
{
  if( unmarked_ready )
    return unmarked;
  unmarked.clear();
  ValuesMap::iterator itEnd = values.end();
  for( ValuesMap::iterator it = values.begin();
       it != itEnd; ++it )
    if( !it.value()->mark )
      unmarked.append( path + it.key() );

  foreach( TreeNode* pNode, nodes ) {
    const QStringList& unmarked_recursive = pNode->GetUnmarked();
    if( !unmarked_recursive.empty() )
      unmarked.append( unmarked_recursive );
  }

  unmarked_ready = true;
  return unmarked;
}

void TreeNode::ResetMarks()
{
  ValuesMap::iterator itEnd = values.end();
  for( ValuesMap::iterator it = values.begin(); it != itEnd; ++it )
    it.value()->mark = false;
  foreach( TreeNode* pNode, nodes )
    pNode->ResetMarks();
  unmarked.clear();
  unmarked_ready = true;
}
