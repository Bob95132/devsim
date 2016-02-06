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

#ifndef MODEL_EXPR_DATA_HH
#define MODEL_EXPR_DATA_HH
#include <memory>
#include <vector>

namespace Eqo {
class EquationObject;
typedef std::shared_ptr<EquationObject> EqObjPtr;
}

template <typename T> class ScalarData;
class NodeModel;
typedef std::shared_ptr<NodeModel>       NodeModelPtr;
typedef std::shared_ptr<const NodeModel> ConstNodeModelPtr;
typedef ScalarData<NodeModel> NodeScalarData;

class EdgeModel;
typedef std::shared_ptr<EdgeModel>       EdgeModelPtr;
typedef std::shared_ptr<const EdgeModel> ConstEdgeModelPtr;
typedef ScalarData<EdgeModel> EdgeScalarData;

class TriangleEdgeModel;
typedef std::shared_ptr<TriangleEdgeModel>       TriangleEdgeModelPtr;
typedef std::shared_ptr<const TriangleEdgeModel> ConstTriangleEdgeModelPtr;
typedef ScalarData<TriangleEdgeModel> TriangleEdgeScalarData;

class TetrahedronEdgeModel;
typedef std::shared_ptr<TetrahedronEdgeModel>       TetrahedronEdgeModelPtr;
typedef std::shared_ptr<const TetrahedronEdgeModel> ConstTetrahedronEdgeModelPtr;
typedef ScalarData<TetrahedronEdgeModel> TetrahedronEdgeScalarData;

class Region;

namespace MEE {
class ModelExprData {
    public:
        enum datatype {
          NODEDATA=0,
          EDGEDATA,
          TRIANGLEEDGEDATA,
          TETRAHEDRONEDGEDATA,
          DOUBLE,
          INVALID
        };

        static const char *const datatypename[];
        explicit ModelExprData(const Region *r = NULL) : val(0.0), type(INVALID), reg(r) {};
        ModelExprData(double x, const Region *r);
        ModelExprData(ConstNodeModelPtr x, const Region *r);
        ModelExprData(ConstEdgeModelPtr x, const Region *r);
        ModelExprData(ConstTriangleEdgeModelPtr x, const Region *r);
        ModelExprData(ConstTetrahedronEdgeModelPtr x, const Region *r);
        ModelExprData(const NodeScalarData &, const Region *);
        ModelExprData(const EdgeScalarData &, const Region *);
        ModelExprData(const TriangleEdgeScalarData &, const Region *);
        ModelExprData(const TetrahedronEdgeScalarData &, const Region *);

        ModelExprData(const ModelExprData &);
        ModelExprData &operator=(const ModelExprData &);

        template <typename T>
        ModelExprData &op_equal(const ModelExprData &, const T &);

        ModelExprData &operator*=(const ModelExprData &);
        ModelExprData &operator+=(const ModelExprData &);

        // test or will get undefined data
        double GetDoubleValue() const
        {
            return val;
        }
        datatype GetType() const {
            return type;
        }

        class ScalarValuesType {
          public:
          ScalarValuesType() : vals(0), uval(0.0), isuniform(false), length(0) {}
          ScalarValuesType(const std::vector<double> &v) : vals(&v), uval(0.0), isuniform(false) {length = v.size();}

          ScalarValuesType(double v, size_t len) : vals(0), uval(v), isuniform(true), length(len) {}

          bool IsUniform() const
          {
            return isuniform;
          }

          const std::vector<double> &GetVector() const {
            return *vals;
          }

          double GetScalar() const {
            return uval;
          }

          size_t GetLength() const {
            return length;
          }

          private:
            const std::vector<double> *vals;
            double                     uval;
            bool isuniform;
            size_t                     length;
        };

        ScalarValuesType GetScalarValues() const;

        void convertToTriangleEdgeData();
        void convertToTetrahedronEdgeData();

    private:
        template <typename T>
        void node_op_equal(const ModelExprData &, const T &);
        template <typename T>
        void edge_op_equal(const ModelExprData &, const T &);
        template <typename T>
        void triangle_edge_op_equal(const ModelExprData &, const T &);
        template <typename T>
        void tetrahedron_edge_op_equal(const ModelExprData &, const T &);
        template <typename T>
        void double_op_equal(const ModelExprData &, const T &);




        friend class ModelExprEval;

        /// use member function pointer to make sure that we don't do
        /// redundant operations
/** Address this in the future when it makes sense to do so
        typedef ModelExprData &(ModelExprData::*selfopptr)(ModelExprData &);
        ModelExprData &DoSelfOp(const ModelExprData &, selfopptr);
*/

//      void morph();
        typedef ModelExprData &(ModelExprData::*selfopptr)(ModelExprData &);

        typedef std::shared_ptr<NodeScalarData>            nodeScalarData_ptr;
        typedef std::shared_ptr<EdgeScalarData>            edgeScalarData_ptr;
        typedef std::shared_ptr<TriangleEdgeScalarData>    triangleEdgeScalarData_ptr;
        typedef std::shared_ptr<TetrahedronEdgeScalarData> tetrahedronEdgeScalarData_ptr;
        // These are the types of data we can keep
        nodeScalarData_ptr            nodeScalarData;
        edgeScalarData_ptr            edgeScalarData;
        triangleEdgeScalarData_ptr    triangleEdgeScalarData;
        tetrahedronEdgeScalarData_ptr tetrahedronEdgeScalarData;
        double   val;

        datatype type;
        const    Region *          reg;

};
}
#endif
