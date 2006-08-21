#include <inttypes.h>
#include <exception>
#include "cda_compiler_support.h"
#include "IfaceCellML_APISPEC.hxx"
#include "IfaceCCGS.hxx"
#include "CCGSBootstrap.hpp"
#include "CellMLBootstrap.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

char*
TypeToString(iface::cellml_services::VariableEvaluationType vet)
{
  switch (vet)
  {
  case iface::cellml_services::BOUND:
    return "bound variable";
  case iface::cellml_services::CONSTANT:
    return "constant";
  case iface::cellml_services::COMPUTED_CONSTANT:
    return "computed once";
  case iface::cellml_services::DIFFERENTIAL:
    return "differential";
  case iface::cellml_services::COMPUTED:
    return "computed for every bound variable";
  }

  return "invalid type";
}

void
WriteCode(iface::cellml_services::CCodeInformation* cci)
{
  iface::cellml_services::ModelConstraintLevel mcl =
    cci->constraintLevel();
  if (mcl == iface::cellml_services::UNDERCONSTRAINED)
  {
    printf("/* Model is underconstrained.\n"
           " * List of undefined variables follows...\n");
    iface::cellml_services::CCodeVariableIterator* ccvi = cci->iterateVariables();
    iface::cellml_services::CCodeVariable* v;
    while (true)
    {
      v = ccvi->nextVariable();
      if (v == NULL)
        break;
      iface::cellml_api::CellMLVariable* sv = v->source();
      wchar_t* n = sv->name();
      printf(" * * %S\n", n);
      free(n);
      sv->release_ref();
      v->release_ref();
    }
    ccvi->release_ref();
    printf(" */\n");
    return;
  }
  else if (mcl == iface::cellml_services::OVERCONSTRAINED)
  {
    printf("/* Model is overconstrained.\n"
           " * List variables defined at time of error follows...\n");
    iface::cellml_services::CCodeVariableIterator* ccvi = cci->iterateVariables();
    iface::cellml_services::CCodeVariable* v;
    while (true)
    {
      v = ccvi->nextVariable();
      if (v == NULL)
        break;
      iface::cellml_api::CellMLVariable* sv = v->source();
      wchar_t* n = sv->name();
      printf(" * * %S\n", n);
      free(n);
      sv->release_ref();
      v->release_ref();
    }
    ccvi->release_ref();
    // Get flagged equations...
    iface::mathml_dom::MathMLNodeList* mnl = cci->flaggedEquations();
    printf(" * Extraneous equation was:\n");
    iface::dom::Node* n = mnl->item(0);
    mnl->release_ref();
    iface::dom::Element* el =
      dynamic_cast<iface::dom::Element*>(n->query_interface("dom::Element"));
    n->release_ref();

    wchar_t* cmeta = el->getAttributeNS(L"http://www.cellml.org/metadata/1.0#",
                                        L"id");
    if (!wcscmp(cmeta, L""))
      printf(" *   <equation with no cmeta ID>\n");
    else
      printf(" *   %S\n", cmeta);
    free(cmeta);

    n = el->parentNode();
    el->release_ref();

    el = dynamic_cast<iface::dom::Element*>
      (n->query_interface("dom::Element"));
    n->release_ref();

    cmeta = el->getAttributeNS(L"http://www.cellml.org/metadata/1.0#", L"id");
    if (!wcscmp(cmeta, L""))
      printf(" *   in <math with no cmeta ID>\n");
    else
      printf(" *   in math with cmeta:id %S\n", cmeta);
    free(cmeta);
    el->release_ref();

    printf(" */\n");
    return;
  }
  printf("/* Model is correctly constrained.\n");
  iface::mathml_dom::MathMLNodeList* mnl = cci->flaggedEquations();
  uint32_t i, l = mnl->length();
  if (l == 0)
    printf(" * No equations needed Newton-Raphson evaluation.\n");
  else
    printf(" * The following equations needed Newton-Raphson evaluation:\n");
  for (i = 0; i < l; i++)
  {
    iface::dom::Node* n = mnl->item(i);
    iface::dom::Element* el =
      dynamic_cast<iface::dom::Element*>(n->query_interface("dom::Element"));
    n->release_ref();

    wchar_t* cmeta = el->getAttributeNS(L"http://www.cellml.org/metadata/1.0#",
                                        L"id");
    if (!wcscmp(cmeta, L""))
      printf(" *   <equation with no cmeta ID>\n");
    else
      printf(" *   %S\n", cmeta);
    free(cmeta);

    n = el->parentNode();
    el->release_ref();

    el = dynamic_cast<iface::dom::Element*>
      (n->query_interface("dom::Element"));
    n->release_ref();

    cmeta = el->getAttributeNS(L"http://www.cellml.org/metadata/1.0#", L"id");
    if (!wcscmp(cmeta, L""))
      printf(" *   in <math with no cmeta ID>\n");
    else
      printf(" *   in math with cmeta:id %S\n", cmeta);
    free(cmeta);
    el->release_ref();
  }
  mnl->release_ref();
  printf(" * The main variable array needs %u entries.\n", cci->variableCount());
  printf(" * The rate array needs %u entries.\n", cci->rateVariableCount());  
  printf(" * The constant array needs %u entries.\n", cci->constantCount());
  printf(" * The bound array needs %u entries.\n", cci->boundCount());
  printf(" * Variable storage is as follows:\n");
  
  iface::cellml_services::CCodeVariableIterator* cvi = cci->iterateVariables();
  while (true)
  {
    iface::cellml_services::CCodeVariable* v = cvi->nextVariable();
    if (v == NULL)
      break;
    iface::cellml_api::CellMLVariable* s = v->source();
    iface::cellml_api::CellMLElement* el = s->parentElement();
    iface::cellml_api::CellMLComponent* c =
      dynamic_cast<iface::cellml_api::CellMLComponent*>
      (el->query_interface("cellml_api::CellMLComponent"));
    el->release_ref();

    wchar_t* vn = s->name(), * cn = c->name();
    printf(" * * Variable %S in component %S\n",
           vn, cn);
    free(vn);
    free(cn);

    c->release_ref();
    s->release_ref();

    printf(" * * * Variable type: %s\n", TypeToString(v->type()));
    printf(" * * * Variable index: %u\n", v->variableIndex());
    printf(" * * * Has differential: %s\n",
           (v->hasDifferential() ? "true" : "false"));
    if (v->hasDifferential())
      printf(" * * * Highest derivative: %u\n", v->derivative());

    v->release_ref();
  }
  cvi->release_ref();

  printf(" */\n");

  char* frag = cci->functionsFragment();
  printf("%s", frag);
  free(frag);

  // Now start the code...
  frag = cci->fixedConstantFragment();
  printf("void SetupFixedConstants(double* CONSTANTS)\n{\n%s}\n", frag);
  free(frag);

  frag = cci->computedConstantFragment();
  printf("void SetupComputedConstants(double* CONSTANTS, double* VARIABLES)\n"
         "{\n%s}\n", frag);
  free(frag);

  frag = cci->rateCodeFragment();
  printf("void ComputeRates(double* BOUND, double* RATES, double* CONSTANTS, "
         "double* VARIABLES)\n"
         "{\n%s}\n", frag);
  free(frag);

  frag = cci->variableCodeFragment();
  printf("void ComputeVariables(double* BOUND, double* RATES, double* CONSTANTS, "
         "double* VARIABLES)\n"
         "{\n%s}\n", frag);
  free(frag);

  
}

int
main(int argc, char** argv)
{
  // Get the URL from which to load the model...
  if (argc < 2)
  {
    printf("Usage: CellML2C modelURL\n");
    return -1;
  }

  wchar_t* URL;
  size_t l = strlen(argv[1]);
  URL = new wchar_t[l + 1];
  memset(URL, 0, (l + 1) * sizeof(wchar_t));
  const char* mbrurl = argv[1];
  mbsrtowcs(URL, &mbrurl, l, NULL);

  iface::cellml_api::CellMLBootstrap* cb =
    CreateCellMLBootstrap();

  iface::cellml_api::ModelLoader* ml =
    cb->modelLoader();
  cb->release_ref();

  iface::cellml_api::Model* mod;
  try
  {
    mod = ml->loadFromURL(URL);
  }
  catch (...)
  {
    printf("Error loading model URL.\n");
    // Well, a leak on exit wouldn't be so bad, but someone might reuse this
    // code, so...
    delete [] URL;
    ml->release_ref();
    return -1;
  }

  ml->release_ref();
  delete [] URL;

  iface::cellml_services::CGenerator* cg =
    CreateCGenerator();

  iface::cellml_services::CCodeInformation* cci = NULL;
  try
  {
    cci = cg->generateCode(mod);
  }
  catch (iface::cellml_api::CellMLException& ce)
  {
    wchar_t* err = cg->lastError();
    printf("Caught a CellMLException while generating code: %S\n", err);
    free(err);
    cg->release_ref();
    mod->release_ref();
    return -1;
  }
  catch (...)
  {
    printf("Unexpected exception calling generateCode!\n");
    // this is a leak, but it should also never happen :)
    return -1;
  }
  mod->release_ref();
  cg->release_ref();

  // We now have the code information...
  WriteCode(cci);
  cci->release_ref();

  return 0;
}
