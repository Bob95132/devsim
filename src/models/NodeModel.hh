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

#ifndef NODEMODEL_HH
#define NODEMODEL_HH

#include <memory>

#include <string>
#include <vector>
#include <iosfwd>

typedef std::vector<double> NodeScalarList;

class NodeModel;
typedef std::weak_ptr<NodeModel>         WeakNodeModelPtr;
typedef std::weak_ptr<const NodeModel>   WeakConstNodeModelPtr;
typedef std::shared_ptr<NodeModel>       NodeModelPtr;
typedef std::shared_ptr<const NodeModel> ConstNodeModelPtr;

class Node;
typedef Node *NodePtr;
typedef const Node *ConstNodePtr;

class Contact;
typedef Contact *ContactPtr;
typedef const Contact *ConstContactPtr;

class Region;
typedef Region *RegionPtr;

class NodeModel {
    public:
        enum DisplayType {NODISPLAY, SCALAR, UNKNOWN};
        NodeModel(const std::string &, const RegionPtr, NodeModel::DisplayType, const ContactPtr x = NULL);
        virtual ~NodeModel()=0;

        const std::string &GetName() const;

        std::string GetRealName() const;

        std::string GetImagName() const;

        // should collapse this with setNodeScalarValue
        const NodeScalarList &GetScalarValues() const;

        const std::vector<size_t> &GetContactIndexes() const;

        // recalculate since dependency is invalid
        void MarkOld();
        bool IsUpToDate() const
        {
            return uptodate;
        }

        /// Use this to break cycles
        /// Only really valid in context of ExprModels.
        bool IsInProcess() const
        {
            return inprocess;
        }

        void InitializeValues();

        void SetNodeValue(size_t, double);
        void SetValues(const NodeScalarList &);
        void SetValues(const NodeModel &);
        void SetValues(const double &);

        const Region &GetRegion() const
        {
            return *myregion;
        }

        bool AtContact() const
        {
            return (mycontact != NULL);
        }

        const Contact &GetContact() const
        {
            return *mycontact;
        }

        const std::string GetContactName() const;

        void SetContact(const ContactPtr);

        NodeModel::DisplayType GetDisplayType() const
        {
          return displayType;
        }

        const char *GetDisplayTypeString() const
        {
          return DisplayTypeString[displayType];
        }

        void SetDisplayType(NodeModel::DisplayType dt)
        {
          displayType = dt;
        }

        ConstNodeModelPtr GetConstSelfPtr() const
        {
          return myself.lock();
        }

        NodeModelPtr GetSelfPtr()
        {
          return myself.lock();
        }

        bool IsUniform() const;

        double GetUniformValue() const;

        size_t GetLength() const
        {
          return length;
        }

        bool IsZero() const
        {
          return (IsUniform() && (uniform_value == 0.0));
        }

        bool IsOne() const
        {
          return (IsUniform() && (uniform_value == 1.0));
        }

        void DevsimSerialize(std::ostream &) const;

        const std::string &GetRegionName() const;

        const std::string &GetDeviceName() const;

    protected:
        //// Devsim Format
        virtual void Serialize(std::ostream &) const = 0;
        void SerializeBuiltIn(std::ostream &) const;

        // Register callbacks in appropriate region
        void RegisterCallback(const std::string &);

        // logically const as is used by the lazy evaluated model values
        void SetValues(const NodeScalarList &) const;
        void SetValues(const double &) const;

        void MarkOld() const;

        void DefaultInitializeValues();


    private:
        void CalculateValues() const;

        //virtual double calcNodeScalarValue(NodePtr) = 0; 
        virtual void calcNodeScalarValues() const = 0; 
        virtual void setInitialValues() = 0;

        NodeModel();
        NodeModel(const NodeModel &);
        NodeModel &operator=(const NodeModel &);

        std::string name;
        WeakNodeModelPtr myself;
        RegionPtr   myregion;
        ContactPtr  mycontact;
        mutable bool uptodate;
        mutable bool inprocess;
        mutable bool isuniform;
        mutable double         uniform_value;
        mutable NodeScalarList values;
        mutable std::vector<size_t> atcontact;
        DisplayType displayType;
        size_t length;
        static const char *DisplayTypeString[];
};
#endif
