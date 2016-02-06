/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "NodeModel.hh"
#include "Device.hh"
#include "Region.hh"
#include "Node.hh"
#include "Contact.hh"
#include "dsAssert.hh"
#include "FPECheck.hh"
#include "GeometryStream.hh"

#include <algorithm>

const char *NodeModel::DisplayTypeString[] = {
  "nodisplay",
  "scalar",
  NULL
};

NodeModel::~NodeModel() {
#if 0
    myregion->UnregisterCallback(name);
#endif
}

// derived classes must register dependencies
NodeModel::NodeModel(const std::string &nm, const RegionPtr rp, NodeModel::DisplayType dt, const ContactPtr cp)
    : name(nm),
      myregion(rp),
      mycontact(cp),
      uptodate(false),
      inprocess(false),
      isuniform(true),
      uniform_value(0.0),
      displayType(dt)
{ 
  myself = rp->AddNodeModel(this);
  length = GetRegion().GetNumberNodes();

  if (mycontact)
  {
    isuniform = false;
  }
}

const std::string &NodeModel::GetName() const
{
  return name;
}

std::string NodeModel::GetRealName() const
{
  return name + "_Real";
}

std::string NodeModel::GetImagName() const
{
  return name + "_Imag";
}

void NodeModel::InitializeValues()
{
  setInitialValues();
}

void NodeModel::DefaultInitializeValues()
{
    // default initialization is to 0.0
  values.clear();
  isuniform     = !mycontact;
  uniform_value = 0.0;
}

void NodeModel::CalculateValues() const
{
  FPECheck::ClearFPE();
  if (!uptodate)
  {
    inprocess = true;
    this->calcNodeScalarValues();
    uptodate = true;
    inprocess = false;
  }

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was a floating point exception of type \"" << FPECheck::getFPEString() << "\"  while evaluating the node model " << name
    << " on Device: " << GetRegion().GetDevice()->GetName() << " on Region: " << GetRegion().GetName() << "\n";
    FPECheck::ClearFPE();
    GeometryStream::WriteOut(OutputStream::FATAL, GetRegion(), os.str().c_str());
  }
}

//
// If not uptodate, then recalculate the model
const NodeScalarList & NodeModel::GetScalarValues() const
{
  CalculateValues();

  if (isuniform)
  {
    dsAssert(!mycontact, "UNEXPECTED");
    values.clear();
    values.resize(length, uniform_value);
  }

  return values;
}

// Note this is logically const and internal to the program
// be careful this is duplicated as a non-const below
void NodeModel::SetValues(const NodeScalarList &nv) const
{
  const_cast<NodeModel *>(this)->SetValues(nv);
}

// be careful this is duplicated as a const above
void NodeModel::SetValues(const NodeScalarList &nv)
{
  if (mycontact)
  {
    values.clear();
    values.resize(length);
    for (std::vector<size_t>::iterator it = atcontact.begin(); it != atcontact.end(); ++it)
    {
      values[*it] = nv[*it];
    }
  }
  else
  {
    values = nv;
  }

  isuniform = false;
  uniform_value = 0.0;

  MarkOld();
  uptodate = true;
}

void NodeModel::SetValues(const NodeModel &nm)
{
  if (&nm != this)
  {
    if (nm.IsUniform())
    {
      const double v = nm.GetUniformValue();
      NodeModel::SetValues(v);
    }
    else
    {
      const NodeScalarList &v = nm.GetScalarValues();
      NodeModel::SetValues(v);
    }
  }
}

void NodeModel::SetValues(const double &v) const
{
  const_cast<NodeModel *>(this)->SetValues(v);
}

void NodeModel::SetValues(const double &v)
{
  if (mycontact)
  {
    isuniform = false;
    values.clear();
    values.resize(length);
    GetContactIndexes(); // safety
    for (std::vector<size_t>::iterator it = atcontact.begin(); it != atcontact.end(); ++it)
    {
      values[*it] = v;
    }
  }
  else
  {
    isuniform = true;
    uniform_value = v;

    values.clear();
  }

  MarkOld();
  uptodate = true;
}

const std::vector<size_t> & NodeModel::GetContactIndexes() const
{
  if ((mycontact) && (atcontact.empty()))
  {
    const ConstNodeList_t &cnodes = mycontact->GetNodes();
    for (ConstNodeList_t::const_iterator it = cnodes.begin(); it != cnodes.end(); ++it)
    {
      atcontact.push_back((*it)->GetIndex());
    }
    std::sort(atcontact.begin(), atcontact.end());
  }

  return atcontact;
}

/*
  if uniform desired then use SetNodeValues above
*/
void NodeModel::SetNodeValue(size_t index, double value)
{
  if (index >= length)
  {
    return;
  }

  //// Set values
  //// unsets uniformity
  GetScalarValues();

  if (isuniform)
  {
    isuniform = false;
    uniform_value = 0.0;
  }

  if (mycontact)
  {
    GetContactIndexes(); // safety
    if (std::find(atcontact.begin(), atcontact.end(), index) != atcontact.end())
    {
      values[index] = value;
    }
  }
  else
  {
    values[index] = value;
  }

  MarkOld();
  uptodate = true;

}

void NodeModel::MarkOld()
{
  uptodate = false;
  myregion->SignalCallbacks(name);
}

void NodeModel::MarkOld() const
{
  const_cast<NodeModel *>(this)->MarkOld();
}

void NodeModel::RegisterCallback(const std::string &nm)
{
  myregion->RegisterCallback(name, nm);
}

bool NodeModel::IsUniform() const
{
  //// We cannot know if we are uniform unless we are uptodate!
  if (!uptodate)
  {
    CalculateValues();
  }

  return isuniform;
}

double NodeModel::GetUniformValue() const
{
  dsAssert(isuniform, "UNEXPECTED");
  return uniform_value;
}

void NodeModel::SerializeBuiltIn(std::ostream &of) const
{
  of << "BUILTIN";
}

void NodeModel::DevsimSerialize(std::ostream &of) const
{
  of << "begin_node_model \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_node_model\n\n";
}

void NodeModel::SetContact(const ContactPtr cp)
{
  mycontact = cp;
  atcontact.clear();
}

const std::string &NodeModel::GetDeviceName() const
{
  return GetRegion().GetDevice()->GetName();
}

const std::string &NodeModel::GetRegionName() const
{
  return GetRegion().GetName();
}

const std::string NodeModel::GetContactName() const
{
  std::string ret;
  if (mycontact)
  {
  ret = mycontact->GetName();
  }
  return ret;
}

