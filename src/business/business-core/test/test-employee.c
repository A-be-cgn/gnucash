#include <glib.h>
#include <guile/gh.h>

#include "guid.h"
#include "gnc-module.h"
#include "gnc-engine-util.h"

#include "gncBusiness.h"
#include "gncEmployee.h"
#include "gncEmployeeP.h"
#include "test-stuff.h"

static int count = 0;

static void
test_string_fcn (GncBusiness *bus, const char *message,
		 void (*set) (GncEmployee *, const char *str),
		 const char * (*get)(GncEmployee *));

static void
test_numeric_fcn (GncBusiness *bus, const char *message,
		  void (*set) (GncEmployee *, gnc_numeric),
		  gnc_numeric (*get)(GncEmployee *));

static void
test_bool_fcn (GncBusiness *bus, const char *message,
		  void (*set) (GncEmployee *, gboolean),
		  gboolean (*get) (GncEmployee *));

static void
test_gint_fcn (GncBusiness *bus, const char *message,
	       void (*set) (GncEmployee *, gint),
	       gint (*get) (GncEmployee *));

static void
test_employee (void)
{
  GncBusiness *bus;
  GncEmployee *employee;

  bus = gncBusinessCreate ((GNCBook *)1);

  /* Test creation/destruction */
  {
    do_test (gncEmployeeCreate (NULL) == NULL, "employee create NULL");
    employee = gncEmployeeCreate (bus);
    do_test (employee != NULL, "employee create");
    do_test (gncEmployeeGetBusiness (employee) == bus,
	     "getbusiness");

    gncEmployeeDestroy (employee);
    success ("create/destroy");
  }

  /* Test setting/getting routines; does the active flag get set right? */
  {
    GUID guid;

    test_string_fcn (bus, "Id", gncEmployeeSetID, gncEmployeeGetID);
    test_string_fcn (bus, "Username", gncEmployeeSetUsername, gncEmployeeGetUsername);
    test_string_fcn (bus, "Language", gncEmployeeSetLanguage, gncEmployeeGetLanguage);
    test_string_fcn (bus, "Acl", gncEmployeeSetAcl, gncEmployeeGetAcl);

    test_numeric_fcn (bus, "Workday", gncEmployeeSetWorkday, gncEmployeeGetWorkday);
    test_numeric_fcn (bus, "Rate", gncEmployeeSetRate, gncEmployeeGetRate);

    test_bool_fcn (bus, "Active", gncEmployeeSetActive, gncEmployeeGetActive);

    do_test (gncEmployeeGetAddr (employee) != NULL, "Addr");

    guid_new (&guid);
    employee = gncEmployeeCreate (bus); count++;
    gncEmployeeSetGUID (employee, &guid);
    do_test (guid_equal (&guid, gncEmployeeGetGUID (employee)), "guid compare");
  }
  {
    GList *list;

    list = gncBusinessGetList (bus, GNC_EMPLOYEE_MODULE_NAME, TRUE);
    do_test (list != NULL, "getList all");
    do_test (g_list_length (list) == count, "correct length: all");
    g_list_free (list);

    list = gncBusinessGetList (bus, GNC_EMPLOYEE_MODULE_NAME, FALSE);
    do_test (list != NULL, "getList active");
    do_test (g_list_length (list) == 1, "correct length: active");
    g_list_free (list);
  }
  {
    const char *str = get_random_string();
    const char *res;

    gncEmployeeSetUsername (employee, str);
    res = gncBusinessPrintable (bus, GNC_EMPLOYEE_MODULE_NAME, employee);
    do_test (res != NULL, "Printable NULL?");
    do_test (safe_strcmp (str, res) == 0, "Printable equals");
  }    
}

static void
test_string_fcn (GncBusiness *bus, const char *message,
		 void (*set) (GncEmployee *, const char *str),
		 const char * (*get)(GncEmployee *))
{
  GncEmployee *employee = gncEmployeeCreate (bus);
  char const *str = get_random_string ();

  do_test (!gncEmployeeIsDirty (employee), "test if start dirty");
  set (employee, str);
  do_test (gncEmployeeIsDirty (employee), "test dirty later");
  do_test (safe_strcmp (get (employee), str) == 0, message);
  gncEmployeeSetActive (employee, FALSE);
  count++;
}

static void
test_numeric_fcn (GncBusiness *bus, const char *message,
		  void (*set) (GncEmployee *, gnc_numeric),
		  gnc_numeric (*get)(GncEmployee *))
{
  GncEmployee *employee = gncEmployeeCreate (bus);
  gnc_numeric num = gnc_numeric_create (17, 1);

  do_test (!gncEmployeeIsDirty (employee), "test if start dirty");
  set (employee, num);
  do_test (gncEmployeeIsDirty (employee), "test dirty later");
  do_test (gnc_numeric_equal (get (employee), num), message);
  gncEmployeeSetActive (employee, FALSE);
  count++;
}

static void
test_bool_fcn (GncBusiness *bus, const char *message,
	       void (*set) (GncEmployee *, gboolean),
	       gboolean (*get) (GncEmployee *))
{
  GncEmployee *employee = gncEmployeeCreate (bus);
  gboolean num = get_random_boolean ();

  do_test (!gncEmployeeIsDirty (employee), "test if start dirty");
  set (employee, FALSE);
  set (employee, TRUE);
  set (employee, num);
  do_test (gncEmployeeIsDirty (employee), "test dirty later");
  do_test (get (employee) == num, message);
  gncEmployeeSetActive (employee, FALSE);
  count++;
}

static void
test_gint_fcn (GncBusiness *bus, const char *message,
	       void (*set) (GncEmployee *, gint),
	       gint (*get) (GncEmployee *))
{
  GncEmployee *employee = gncEmployeeCreate (bus);
  gint num = 17;

  do_test (!gncEmployeeIsDirty (employee), "test if start dirty");
  set (employee, num);
  do_test (gncEmployeeIsDirty (employee), "test dirty later");
  do_test (get (employee) == num, message);
  gncEmployeeSetActive (employee, FALSE);
  count++;
}

static void
main_helper (int argc, char **argv)
{
  gnc_module_load("gnucash/business-core", 0);
  test_employee();
  print_test_results();
  exit(get_rv());
}

int
main (int argc, char **argv)
{
  gh_enter (argc, argv, main_helper);
  return 0;
}