/*******************************************************************************
*
* University of Illinois/NCSA
* Open Source License
*
* Copyright (c) 2018 Advanced Micro Devices, Inc. All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* with the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimers.
*
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimers in the
*       documentation and/or other materials provided with the distribution.
*
*     * Neither the names of Advanced Micro Devices, Inc. nor the names of its
*       contributors may be used to endorse or promote products derived from
*       this Software without specific prior written permission.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH
* THE SOFTWARE.
*
*******************************************************************************/

#ifndef AMD_COMGR_H_
#define AMD_COMGR_H_

#include <stddef.h>   /* size_t */
#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>  /* bool */
#endif /* __cplusplus */

/* Placeholder for calling convention and import/export macros */
#ifndef AMD_CALL
#define AMD_CALL
#endif

#ifndef AMD_EXPORT_DECORATOR
#ifdef __GNUC__
#define AMD_EXPORT_DECORATOR __attribute__ ((visibility ("default")))
#else
#define AMD_EXPORT_DECORATOR __declspec(dllexport)
#endif
#endif

#ifndef AMD_IMPORT_DECORATOR
#ifdef __GNUC__
#define AMD_IMPORT_DECORATOR
#else
#define AMD_IMPORT_DECORATOR __declspec(dllimport)
#endif
#endif

#define AMD_API_EXPORT AMD_EXPORT_DECORATOR AMD_CALL
#define AMD_API_IMPORT AMD_IMPORT_DECORATOR AMD_CALL

#ifndef AMD_API
#ifdef AMD_EXPORT
#define AMD_API AMD_API_EXPORT
#else
#define AMD_API AMD_API_IMPORT
#endif
#endif

#define AMD_COMGR_INTERFACE_VERSION_MAJOR 1
#define AMD_COMGR_INTERFACE_VERSION_MINOR 0

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/** \defgroup codeobjectmanager Code Object Manager
 *  @{
 *
 * @brief The code object manager is a callable library that provides
 * operations for creating and inspecting code objects.
 *
 * The library provides handles to various objects. Concurrent execution of
 * operations is supported provided all objects accessed by each concurrent
 * operation are disjoint. For example, the @p amd_comgr_data_set_t handles
 * passed to operations must be disjoint, together with all the @p
 * amd_comgr_data_t handles that have been added to it. The exception is that
 * the default device library data object handles can be non-disjoint as they
 * are imutable.
 *
 * The library supports generating and inspecting code objects that
 * contain machine code for a certain set of instruction set
 * arhitectures (isa). The set of isa supported and information about
 * the properties of the isa can be queried.
 *
 * The library supports performing an action that can take data
 * objects of one kind, and generate new data objects of another kind.
 *
 * Data objects are referenced using handles using @p
 * amd_comgr_data_t. The kinds of data objects are given
 * by @p amd_comgr_data_kind_t.
 *
 * To perform an action, two @p amd_comgr_data_set_t
 * objects are created. One is used to hold all the data objects
 * needed by an action, and other is updated by the action with all
 * the result data objects. In addition, an @p
 * amd_comgr_action_info_t is created to hold
 * information that controls the action. These are then passed to @p
 * amd_comgr_do_action to perform an action specified by
 * @p amd_comgr_action_kind_t.
 *
 * Some data objects can have associated metadata. There are
 * operations for querying this metadata.
 *
 * The default device library that satisfies the requirements of the
 * compiler action can be obtained.
 */

/**
 * @brief Status codes.
 */
typedef enum amd_comgr_status_s {
  /**
   * The function has been executed successfully.
   */
  AMD_COMGR_STATUS_SUCCESS = 0x0,
  /**
   * A generic error has occurred.
   */
  AMD_COMGR_STATUS_ERROR = 0x1,
  /**
   * One of the actual arguments does not meet a precondition stated
   * in the documentation of the corresponding formal argument.
   */
  AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT = 0x2,
  /**
   * Failed to allocate the necessary resources.
   */
  AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES = 0x3,
} amd_comgr_status_t;

/**
 * @brief The source languages supported by the compiler.
 */
typedef enum amd_comgr_language_s {
  /**
   * No high level language.
   */
  AMD_COMGR_LANGUAGE_NONE,
  /**
   * OpenCL 1.2.
   */
  AMD_COMGR_LANGUAGE_OPENCL_1_2,
  /**
   * OpenCL 2.0.
   */
  AMD_COMGR_LANGUAGE_OPENCL_2_0,
  /**
   * AMD Hetrogeneous C++ (HC).
   */
  AMD_COMGR_LANGUAGE_HC,
  /**
   * Marker for last valid language.
   */
  AMD_COMGR_LANGUAGE_LAST = AMD_COMGR_LANGUAGE_HC
} amd_comgr_language_t;

/**
 * @brief Query additional information about a status code.
 *
 * @param[in] status Status code.
 *
 * @param[out] status_string A NUL-terminated string that describes
 * the error status.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * status is an invalid status code, or @p status_string is NULL.
 */
amd_comgr_status_t AMD_API amd_comgr_status_string(
    amd_comgr_status_t status,
    const char ** status_string);

/**
 * @brief Get the version of the code object manager interface
 * supported.
 *
 * An interface is backwards compatible with an implementation with an
 * equal major version, and a greater than or equal minor version.
 *
 * @param[out] major Major version number.
 *
 * @param[out] minor Minor version number.
 */
void AMD_API amd_comgr_get_version(
  size_t *major,
  size_t *minor);

/**
 * @brief The kinds of data supported.
 */
typedef enum amd_comgr_data_kind_s {
  /**
   * No data is available.
   */
  AMD_COMGR_DATA_KIND_UNDEF,
  /**
   * The data is a textual main source.
   */
  AMD_COMGR_DATA_KIND_SOURCE,
  /**
   * The data is a textual source that is included in the main source
   * or other include source.
   */
  AMD_COMGR_DATA_KIND_INCLUDE,
  /**
   * The data is a precompiled-header source that is included in the main
   * source or other include source.
   */
  AMD_COMGR_DATA_KIND_PRECOMPILED_HEADER,
  /**
   * The data is a diagnostic output.
   */
  AMD_COMGR_DATA_KIND_DIAGNOSTIC,
  /**
   * The data is a textual log output.
   */
  AMD_COMGR_DATA_KIND_LOG,
  /**
   * The data is compiler LLVM IR bit code for a specific isa.
   */
  AMD_COMGR_DATA_KIND_BC,
  /**
   * The data is a relocatable machine code object for a specific isa.
   */
  AMD_COMGR_DATA_KIND_RELOCATABLE,
  /**
   * The data is an executable machine code object for a specific
   * isa. An executable is the kind of code object that can be loaded
   * and executed.
   */
  AMD_COMGR_DATA_KIND_EXECUTABLE,
  /**
   * The data is a block of bytes.
   */
  AMD_COMGR_DATA_KIND_BYTES,
  /**
   * Marker for last valid data kind.
   */
  AMD_COMGR_DATA_KIND_LAST = AMD_COMGR_DATA_KIND_BYTES
} amd_comgr_data_kind_t;

/**
 * @brief A handle to a data object.
 *
 * Data objects are used to hold the data which is either an input or
 * output of a code object manager action.
 */
typedef struct amd_comgr_data_s {
  uint64_t handle;
} amd_comgr_data_t;

/**
 * @brief A handle to an action data object.
 *
 * An action data object holds a set of data objects. These can be
 * used as inputs to an action, or produced as the result of an
 * action.
 */
typedef struct amd_comgr_data_set_s {
  uint64_t handle;
} amd_comgr_data_set_t;

/**
 * @brief A handle to an action information object.
 *
 * An action information object holds all the necessary information,
 * excluding the input data objects, required to perform an action.
 */
typedef struct amd_comgr_action_info_s {
  uint64_t handle;
} amd_comgr_action_info_t;

/**
 * @brief A handle to a metadata node.
 *
 * A metadata node handle is used to traverse the metadata associated
 * with a data node.
 */
typedef struct amd_comgr_metadata_node_s {
  uint64_t handle;
} amd_comgr_metadata_node_t;

/**
 * @brief A handle to a machine code object symbol.
 *
 * A symbol handle is used to obtain the properties of symbols of a machine code
 * object. A symbol handle is invalidated when the data object containing the
 * symbol is destroyed.
 */
typedef struct amd_comgr_symbol_s {
  uint64_t handle;
} amd_comgr_symbol_t;

/**
 * @brief Return the number of isa names supported by this version of
 * the code object manager library.
 *
 * The isa name specifies the instruction set architecture that should
 * be used in the actions that involve machine code generation or
 * inspection.
 *
 * @param[out] count The number of isa names supported.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * count is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update action info object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_isa_count(
  size_t *count);

/**
 * @brief Return the Nth isa name supported by this version of the
 * code object manager library.
 *
 * @param[in] index The index of the isa name to be returned. The
 * first isa name is index 0.
 *
 * @param[out] isa_name A null terminated string that is the isa name
 * being requested.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * index is greater than the number of isa name supported by this
 * version of the code object manager library. @p isa_name is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update action info object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_isa_name(
  size_t index,
  const char **isa_name);

 /**
 * @brief Get a handle to the metadata of an isa name.
 *
 * The structure of the returned metadata is isa name specific. It can
 * include information about the limits for resources such as
 * registers and memory addressing.
 *
 * @param[in] isa_name The isa name to query.
 *
 * @param[out] metadata A handle to the metadata of the isa name. If
 * the isa name has no metadata then the returned handle has a kind of
 * @p AMD_COMGR_METADATA_KIND_NULL. The handle must be destroyed
 * using @c amd_comgr_destroy_metadata.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * name is NULL or is not an isa name supported by this version of the
 * code object manager library. @p metadata is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_isa_metadata(
  const char *isa_name,
  amd_comgr_metadata_node_t *metadata);

 /**
 * @brief Add the default device library data objects for a specific
 * isa name, isa specific data kind and language to an existing action
 * data object.
 *
 * Zero or more such device libraries may be added which can be linked
 * with other data objects to provide the functionality expected by
 * the data objects created by the compiler for the specified
 * language.
 *
 * @param[in] isa_name The isa name to query.
 *
 * @param[in] kind The kind of data object requested.
 *
 * @param[in] language The source language for which the default
 * device libraries are requested. If @p
 * AMD_COMGR_LANGUAGE_NONE is specified only the base
 * language independent libraries will be returned.
 *
 * @param[in, out] result If the @p kind is isa specific, then add the
 * default device libraries of that lind for language @p language if
 * any exist. If a device library is already present then it is not
 * added a second time. This allows the device libraries for multiple
 * languages to be added by using this operation multiple times on the
 * same @p result, where the same device library may be required by
 * multiple languages.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * name is NULL or is not an isa name supported by this version of the
 * code object manager library. @p kind is an invalid data kind, or @p
 * AMD_COMGR_DATA_KIND_UNDEF. @p language is an invalid
 * language kind. @p result is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_add_isa_default_device_libraries(
  const char *isa_name,
  amd_comgr_data_kind_t kind,
  amd_comgr_language_t language,
  amd_comgr_data_set_t *result);

/**
 * @brief Create a data object that can hold data of a specified kind.
 *
 * Data objects are reference counted and are destroyed when the
 * reference count reaches 0. When a data object is created its
 * reference count is 1, it has 0 bytes of data, it has an empty name,
 * and it has no metadata.
 *
 * @param[in] kind The kind of data the object is intended to hold.
 *
 * @param[out] data A handle to the data object created. Its reference
 * count is set to 1.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * kind is an invalid data kind, or @p
 * AMD_COMGR_DATA_KIND_UNDEF. @p data is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to create the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_create_data(
  amd_comgr_data_kind_t kind,
  amd_comgr_data_t *data);

 /**
 * @brief Indicate that no longer using a data object handle.
 *
 * The reference count of the associated data object is
 * decremented. If it reaches 0 it is destroyed.
 *
 * @param[in] data The data object to release.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * data is an invalid data object, or has kind @p
 * AMD_COMGR_DATA_KIND_UNDEF.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_release_data(
  amd_comgr_data_t data);

/**
 * @brief Get the kind of the data object.
 *
 * @param[in] data The data object to query.
 *
 * @param[out] kind The kind of data the object.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * data is an invalid data object. @p kind is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to create the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_data_kind(
  amd_comgr_data_t data,
  amd_comgr_data_kind_t *kind);

/**
 * @brief Set the data content of a data object to the specified
 * bytes.
 *
 * Any previous value of the data object is overwritten. Any metadata
 * associated with the data object is also replaced which invalidates
 * all metadata handles to the old metadata.
 *
 * @param[in] data The data object to update.
 *
 * @param[in] size The number of bytes in the data specified by @p bytes.
 *
 * @param[in] bytes The bytes to set the data object to. The bytes are
 * copied into the data object and can be freed after the call.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * data is an invalid data object, or has kind @p
 * AMD_COMGR_DATA_KIND_UNDEF.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_set_data(
  amd_comgr_data_t data,
  size_t size,
  const char* bytes);

/**
 * @brief Set the name associated with a data object.
 *
 * When compiling, the fle name of an include directive is used to
 * reference the contents of the include data object with the same
 * name. The name may also be used for other data objects in log and
 * diagnostic output.
 *
 * @param[in] data The data object to update.
 *
 * @param[in] name A null terminated string that specifies the name to
 * use for the data object. If NULL then the name is set to the empty
 * string.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * data is an invalid data object, or has kind @p
 * AMD_COMGR_DATA_KIND_UNDEF.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_set_data_name(
  amd_comgr_data_t data,
  const char* name);

/**
 * @brief Get the data contents, and/or the size of the data
 * associated with a data object.
 *
 * @param[in] data The data object to query.
 *
 * @param[in, out] size On entry, the size of @p bytes. On return, set
 * to the size of the data object contents.
 *
 * @param[out] bytes If not NULL, then the first @p size bytes of the
 * data object contents is copied. If NULL, no data is copied, and
 * only @p size is updated (useful inorder to find the size of buffer
 * required to copy the data).
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * data is an invalid data object, or has kind @p
 * AMD_COMGR_DATA_KIND_UNDEF. @p size is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_data(
  amd_comgr_data_t data,
  size_t *size,
  char *bytes);

/**
 * @brief Get the data object name and/or name length.
 *
 * @param[in] data The data object to query.
 *
 * @param[in, out] size On entry, the size of @p name. On return, set
 * to the size of the data object name including the terminating null
 * character.
 *
 * @param[out] name If not NULL, then the first @p size characters of the
 * data object name are copied. If NULL, no name is copied, and
 * only @p size is updated (useful inorder to find the size of buffer
 * required to copy the name).
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * data is an invalid data object, or has kind @p
 * AMD_COMGR_DATA_KIND_UNDEF. @p size is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_data_name(
  amd_comgr_data_t data,
  size_t *size,
  char *name);

/**
 * @brief Get the data object isa name and/or isa name length.
 *
 * @param[in] data The data object to query.
 *
 * @param[in, out] size On entry, the size of @p isa_name. On return,
 * set to the size of the isa name including the terminating null
 * character.
 *
 * @param[out] isa_name If not NULL, then the first @p size characters
 * of the isa name are copied. If NULL, no isa name is copied, and
 * only @p size is updated (useful inorder to find the size of buffer
 * required to copy the isa name).
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * data is an invalid data object, has kind @p
 * AMD_COMGR_DATA_KIND_UNDEF, or is not an isa specific
 * kind. @p size is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_data_isa_name(
  amd_comgr_data_t data,
  size_t *size,
  char *isa_name);

 /**
 * @brief Get a handle to the metadata of a data object.
 *
 * @param[in] data The data object to query.
 *
 * @param[out] metadata A handle to the metadata of the data
 * object. If the data object has no metadata then the returned handle
 * has a kind of @p AMD_COMGR_METADATA_KIND_NULL. The
 * handle must be destroyed using @c amd_comgr_destroy_metadata.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * data is an invalid data object, or has kind @p
 * AMD_COMGR_DATA_KIND_UNDEF. @p metadata is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_data_metadata(
  amd_comgr_data_t data,
  amd_comgr_metadata_node_t *metadata);

/**
 * @brief Destroy a metadata handle.
 *
 * @param[in] metadata A metadata handle to destroy.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has been executed
 * successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p metadata is an invalid
 * metadata handle.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES Unable to update metadata
 * handle as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_destroy_metadata(amd_comgr_metadata_node_t metadata);

/**
 * @brief Create a data set object.
 *
 * @param[out] data_set A handle to the data set created. Initially it
 * contains no data objects.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has been executed
 * successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p data_set is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES Unable to create the data
 * set object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_create_data_set(
  amd_comgr_data_set_t *data_set);

/**
 * @brief Destroy a data set object.
 *
 * The reference counts of any associated data objects are decremented. Any
 * handles to the data set object become invalid.
 *
 * @param[in] data_set A handle to the data set object to destroy.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has been executed
 * successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p data_set is an invalid
 * data set object.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES Unable to update data set
 * object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_destroy_data_set(
  amd_comgr_data_set_t data_set);

/**
 * @brief Add a data object to a data set object if it is not already added.
 *
 * The reference count of the data object is incremented.
 *
 * @param[in] data_set A handle to the data set object to be updated.
 *
 * @param[in] data A handle to the data object to be added. If @p data_set
 * already has the specified handle present, then it is not added. The order
 * that data objects are added is preserved.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has been executed
 * successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p data_set is an invalid
 * data set object. @p data is an invalid data object; has undef kind; has
 * include kind but does not have a name.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES Unable to update data set
 * object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_data_set_add(
  amd_comgr_data_set_t data_set,
  amd_comgr_data_t data);

/**
 * @brief Remove all data objects of a specified kind from a data set object.
 *
 * The reference count of the removed data objects is decremented.
 *
 * @param[in] data_set A handle to the data set object to be updated.
 *
 * @param[in] data_kind The data kind of the data objects to be removed. If @p
 * AMD_COMGR_DATA_KIND_UNDEF is specified then all data objects are removed.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has been executed
 * successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p data_set is an invalid
 * data set object. @p data_kind is an invalid data kind.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES Unable to update data set
 * object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_data_set_remove(
  amd_comgr_data_set_t data_set,
  amd_comgr_data_kind_t data_kind);

/**
 * @brief Return the number of data objects of a specified data kind that are
 * added to a data set object.
 *
 * @param[in] data_set A handle to the data set object to be queried.
 *
 * @param[in] data_kind The data kind of the data objects to be counted.
 *
 * @param[out] count The number of data objects of data kind @p data_kind.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has been executed
 * successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p data_set is an invalid
 * data set object. @p data_kind is an invalid data kind or @p
 * AMD_COMGR_DATA_KIND_UNDEF. @p count is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES Unable to query data set
 * object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_data_count(
  amd_comgr_data_set_t data_set,
  amd_comgr_data_kind_t data_kind,
  size_t *count);

/**
 * @brief Return the Nth data object of a specified data kind that is added to a
 * data set object.
 *
 * The reference count of the returned data object is incremented.
 *
 * @param[in] data_set A handle to the data set object to be queried.
 *
 * @param[in] data_kind The data kind of the data object to be returned.
 *
 * @param[in] index The index of the data object of data kind @data_kind to be
 * returned. The first data object is index 0. The order of data objects matches
 * the order that they were added to the data set object.
 *
 * @param[out] data The data object being requested.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has been executed
 * successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p data_set is an invalid
 * data set object. @p data_kind is an invalid data kind or @p
 * AMD_COMGR_DATA_KIND_UNDEF. @p index is greater than the number of data
 * objects of kind @p data_kind. @p data is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES Unable to query data set
 * object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_data_get_data(
  amd_comgr_data_set_t data_set,
  amd_comgr_data_kind_t data_kind,
  size_t index,
  amd_comgr_data_t *data);

/**
 * @brief Create an action info object.
 *
 * @param[out] action_info A handle to the action info object created.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to create the action info object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_create_action_info(
  amd_comgr_action_info_t *action_info);

/**
 * @brief Destroy an action info object.
 *
 * @param[in] action_info A handle to the action info object to destroy.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update action info object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_destroy_action_info(
  amd_comgr_action_info_t action_info);

/**
 * @brief Set the isa name of an action info object.
 *
 * When an action info object is created it has no isa name. Some
 * actions require that the action info object has an isa name
 * defined.
 *
 * @param[in] action_info A handle to the action info object to be
 * updated.
 *
 * @param[in] isa_name A null terminated string that is the isa name. If NULL
 * or the empty string then the isa name is cleared. The isa name is defined as
 * the Code Object Target Identification string, described at
 * https://llvm.org/docs/AMDGPUUsage.html#code-object-target-identification
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object. @p isa_name is not an
 * isa name supported by this version of the code object manager
 * library.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update action info object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_set_isa_name(
  amd_comgr_action_info_t action_info,
  const char *isa_name);

/**
 * @brief Get the isa name and/or isa name length.
 *
 * @param[in] action_info The action info object to query.
 *
 * @param[in, out] size On entry, the size of @p isa_name. On return,
 * set to the size of the isa name including the terminating null
 * character.
 *
 * @param[out] isa_name If not NULL, then the first @p size characters
 * of the isa name are copied. If the isa name is not set then an
 * empty string is copied. If NULL, no name is copied, and only @p
 * size is updated (useful inorder to find the size of buffer required
 * to copy the name).
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object. @p size is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_get_isa_name(
  amd_comgr_action_info_t action_info,
  size_t *size,
  char *isa_name);

/**
 * @brief Set the source language of an action info object.
 *
 * When an action info object is created it has no language defined
 * which is represented by @p
 * AMD_COMGR_LANGUAGE_NONE. Some actions require that
 * the action info object has a source language defined.
 *
 * @param[in] action_info A handle to the action info object to be
 * updated.
 *
 * @param[in] language The language to set. If @p
 * AMD_COMGR_LANGUAGE_NONE then the language is cleared.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object. @p language is an
 * invalid language.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update action info object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_set_language(
  amd_comgr_action_info_t action_info,
  amd_comgr_language_t language);

/**
 * @brief Get the language for an action info object.
 *
 * @param[in] action_info The action info object to query.
 *
 * @param[out] language The language of the action info opject. @p
 * AMD_COMGR_LANGUAGE_NONE if not defined,
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object. @p language is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_get_language(
  amd_comgr_action_info_t action_info,
  amd_comgr_language_t *language);

/**
 * @brief Set the options of an action info object.
 *
 * When an action info object is created it has empty options.
 *
 * @param[in] action_info A handle to the action info object to be
 * updated.
 *
 * @param[in] options A null terminated string that is the options. If
 * NULL or the empty string then the options are cleared.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update action info object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_set_options(
  amd_comgr_action_info_t action_info,
  const char *options);

/**
 * @brief Get the options and/or isa options length of an action info
 * object.
 *
 * @param[in] action_info The action info object to query.
 *
 * @param[in, out] size On entry, the size of @p options. On return,
 * set to the size of the options including the terminating null
 * character.
 *
 * @param[out] options If not NULL, then the first @p size characters of
 * the options are copied. If the options are not set then an empty
 * string is copied. If NULL, options is not copied, and only @p size
 * is updated (useful inorder to find the size of buffer required to
 * copy the options).
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object. @p size is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_get_options(
  amd_comgr_action_info_t action_info,
  size_t *size,
  char *options);

/**
 * @brief Set the working directory of an action info object.
 *
 * When an action info object is created it has an empty working
 * directory. Some actions use the working directory to resolve
 * relative file paths.
 *
 * @param[in] action_info A handle to the action info object to be
 * updated.
 *
 * @param[in] path A null terminated string that is the working
 * directory path. If NULL or the empty string then the working
 * directory is cleared.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update action info object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_set_working_directory_path(
  amd_comgr_action_info_t action_info,
  const char *path);

/**
 * @brief Get the working directory path and/or working directory path
 * length of an action info object.
 *
 * @param[in] action_info The action info object to query.
 *
 * @param[in, out] size On entry, the size of @p path. On return, set
 * to the size of the working directory path including the terminating
 * null character.
 *
 * @param[out] path If not NULL, then the first @p size characters of
 * the working directory path is copied. If the working directory path
 * is not set then an empty string is copied. If NULL, the working
 * directory path is not copied, and only @p size is updated (useful
 * inorder to find the size of buffer required to copy the working
 * directory path).
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object. @p size is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_get_working_directory_path(
  amd_comgr_action_info_t action_info,
  size_t *size,
  char *path);

/**
 * @brief Set whether logging is enabled for an action info object.
 *
 * @param[in] action_info A handle to the action info object to be
 * updated.
 *
 * @param[in] logging Whether logging should be enabled or disable.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update action info object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_set_logging(
  amd_comgr_action_info_t action_info,
  bool logging);

/**
 * @brief Get whether logging is enabled for an action info object.
 *
 * @param[in] action_info The action info object to query.
 *
 * @param[out] logging Whether logging is enabled.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * action_info is an invalid action info object. @p logging is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_action_info_get_logging(
  amd_comgr_action_info_t action_info,
  bool *logging);

/**
 * @brief The kinds of actions that can be performed.
 */
typedef enum amd_comgr_action_kind_s {
  /**
   * Preprocess each source data object in @p input in order. For each
   * successful preprocessor invocation, add a source data object to @p result.
   * Resolve any include source names using the names of include data objects
   * in @p input. Resolve any include relative path names using the working
   * directory path in @p info. Preprocess the source for the language in @p
   * info.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if any preprocessing fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name or language is not set in @p info.
   */
  AMD_COMGR_ACTION_SOURCE_TO_PREPROCESSOR,
  /**
   * Compile each source data object in @p input in order. For each
   * successful compilation add a bc data object to @p result. Resolve
   * any include source names using the names of include data objects
   * in @p input. Resolve any include relative path names using the
   * working directory path in @p info. Produce bc for isa name in @p
   * info. Compile the source for the language in @p info.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if any compilation
   * fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name or language is not set in @p info.
   */
  AMD_COMGR_ACTION_COMPILE_SOURCE_TO_BC,
  /**
   * Link each bc data object in @p input together and add the linked
   * bc data object to @p result. Any device library bc data object
   * must be explicitly added to @p input if needed.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if the link fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name is not set in @p info and does not match the isa name
   * of all bc data objects in @p input.
   */
  AMD_COMGR_ACTION_LINK_BC_TO_BC,
  /**
   * Optimize each bc data object in @p input and create an optimized bc data
   * object to @p result.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if the optimization fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name is not set in @p info and does not match the isa name
   * of all bc data objects in @p input.
   */
  AMD_COMGR_ACTION_OPTIMIZE_BC_TO_BC,
  /**
   * Perform code generation for each bc data object in @p input in
   * order. For each successful code generation add a relocatable data
   * object to @p result.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if any code
   * generation fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name is not set in @p info and does not match the isa name
   * of all bc data objects in @p input.
   */
  AMD_COMGR_ACTION_CODEGEN_BC_TO_RELOCATABLE,
  /**
   * Perform code generation for each bc data object in @p input in
   * order. For each successful code generation add an assembly source data
   * object to @p result.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if any code
   * generation fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name is not set in @p info and does not match the isa name
   * of all bc data objects in @p input.
   */
  AMD_COMGR_ACTION_CODEGEN_BC_TO_ASSEMBLY,
  /**
   * Link each relocatable data object in @p input together and add
   * the linked relocatable data object to @p result. Any device
   * library relocatable data object must be explicitly added to @p
   * input if needed.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if the link fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name is not set in @p info and does not match the isa name
   * of all relocatable data objects in @p input.
   */
  AMD_COMGR_ACTION_LINK_RELOCATABLE_TO_RELOCATABLE,
  /**
   * Link each relocatable data object in @p input together and add
   * the linked executable data object to @p result. Any device
   * library relocatable data object must be explicitly added to @p
   * input if needed.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if the link fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name is not set in @p info and does not match the isa name
   * of all relocatable data objects in @p input.
   */
  AMD_COMGR_ACTION_LINK_RELOCATABLE_TO_EXECUTABLE,
  /**
   * Assemble each source data object in @p input in order into machine code.
   * For each successful assembly add a relocatable data object to @p result.
   * Resolve any include source names using the names of include data objects in
   * @p input. Resolve any include relative path names using the working
   * directory path in @p info. Produce relocatable for isa name in @p info.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if any assembly fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT if isa name is not set in
   * @p info.
   */
  AMD_COMGR_ACTION_ASSEMBLE_SOURCE_TO_RELOCATABLE,
  /**
   * Disassemble each relocatable data object in @p input in
   * order. For each successful disassembly add a source data object to
   * @p result.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if any disassembly
   * fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name is not set in @p info and does not match the isa name
   * of all relocatable data objects in @p input.
   */
  AMD_COMGR_ACTION_DISASSEMBLE_RELOCATABLE_TO_SOURCE,
  /**
   * Disassemble each executable data object in @p input in order. For
   * each successful disassembly add a source data object to @p result.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if any disassembly
   * fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name is not set in @p info and does not match the isa name
   * of all relocatable data objects in @p input.
   */
  AMD_COMGR_ACTION_DISASSEMBLE_EXECUTABLE_TO_SOURCE,
  /**
   * Disassemble each bytes data object in @p input in order. For each
   * successful disassembly add a source data object to @p
   * result. Only simple assembly language commands are generate that
   * corresponf to raw bytes are supported, not any directives that
   * control the code object layout, or symbolic branch targets or
   * names.
   *
   * Return @p AMD_COMGR_STATUS_ERROR if any disassembly
   * fails.
   *
   * Return @p AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT
   * if isa name is not set in @p info
   */
  AMD_COMGR_ACTION_DISASSEMBLE_BYTES_TO_SOURCE,
  /**
   * Marker for last valid action kind.
   */
  AMD_COMGR_ACTION_LAST = AMD_COMGR_ACTION_DISASSEMBLE_BYTES_TO_SOURCE
} amd_comgr_action_kind_t;

/**
 * @brief Perform an action.
 *
 * Each action ignores any data objects in @p input that it does not
 * use. If logging is enabled in @info then @p result will have a log
 * data object added. Any diagnostic data objects produced by the
 * action will be added to @p result. See the description of each
 * action in @p amd_comgr_action_kind_t.
 *
 * @param[in] kind The action to perform.
 *
 * @param[in] info The action info to use when performing the action.
 *
 * @param[in] input The input data objects to the @p kind action.
 *
 * @param[out] result Any data objects are removed before performing
 * the action which then adds all data objects produced by the action.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR An error was
 * reported when executing the action.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * kind is an invalid action kind. @p input_data or @p result_data are
 * invalid action data object handles. See the description of each
 * action in @p amd_comgr_action_kind_t for other
 * conditions that result in this status.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_do_action(
  amd_comgr_action_kind_t kind,
  amd_comgr_action_info_t info,
  amd_comgr_data_set_t input,
  amd_comgr_data_set_t result);

/**
 * @brief The kinds of metadata nodes.
 */
typedef enum amd_comgr_metadata_kind_s {
  /**
   * The NULL metadata handle.
   */
  AMD_COMGR_METADATA_KIND_NULL = 0,
  /**
   * A sting value.
   */
  AMD_COMGR_METADATA_KIND_STRING = 1,
  /**
   * A map that consists of a set of key and value pairs.
   */
  AMD_COMGR_METADATA_KIND_MAP = 2,
  /**
   * A list that consists of a sequence of values.
   */
  AMD_COMGR_METADATA_KIND_LIST = 3,
  /**
   * Marker for last valid metadata kind.
   */
  AMD_COMGR_METADATA_KIND_LAST = AMD_COMGR_METADATA_KIND_LIST
} amd_comgr_metadata_kind_t;

/**
 * @brief Get the kind of the metadata node.
 *
 * @param[in] metadata The metadata node to query.
 *
 * @param[out] kind The kind of the metadata node.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * metadata is an invalid metadata node. @p kind is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to create the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_metadata_kind(
  amd_comgr_metadata_node_t metadata,
  amd_comgr_metadata_kind_t *kind);

/**
 * @brief Get the string and/or string length from a metadata string
 * node.
 *
 * @param[in] metadata The metadata node to query.
 *
 * @param[in, out] size On entry, the size of @p string. On return,
 * set to the size of the sring including the terminating null
 * character.
 *
 * @param[out] string If not NULL, then the first @p size characters
 * of the string are copied. If NULL, no string is copied, and only @p
 * size is updated (useful inorder to find the size of buffer required
 * to copy the string).
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * metadata is an invalid metadata node, or does not have kind @p
 * AMD_COMGR_METADATA_KIND_STRING. @p size is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_metadata_string(
  amd_comgr_metadata_node_t metadata,
  size_t *size,
  char *string);

/**
 * @brief Get the map size from a metadata map node.
 *
 * @param[in] metadata The metadata node to query.
 *
 * @param[out] size The number of entries in the map.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * metadata is an invalid metadata node, or not of kind @p
 * AMD_COMGR_METADATA_KIND_MAP. @p size is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_metadata_map_size(
  amd_comgr_metadata_node_t metadata,
  size_t *size);

/**
 * @brief Iterate over the elements a metadata map node.
 *
 * @warning The metadata nodes which are passed to the callback are not owned
 * by the callback, and are freed just after the callback returns. The callback
 * must not save any references to its parameters between iterations.
 *
 * @param[in] metadata The metadata node to query.
 *
 * @param[in] callback The function to call for each entry in the map. The
 * entry's key is passed in @p key, the entry's value is passed in @p value, and
 * @p user_data is passed as @p user_data. If the function returns with a status
 * other than @p AMD_COMGR_STATUS_SUCCESS then iteration is stopped.
 *
 * @param[in] user_data The value to pass to each invocation of @p
 * callback. Allows context to be passed into the call back function.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR An error was
 * reported by @p callback.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * metadata is an invalid metadata node, or not of kind @p
 * AMD_COMGR_METADATA_KIND_MAP. @p callback is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to iterate the metadata as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_iterate_map_metadata(
  amd_comgr_metadata_node_t metadata,
  amd_comgr_status_t (*callback)(
    amd_comgr_metadata_node_t key,
    amd_comgr_metadata_node_t value,
    void *user_data),
  void *user_data);

/**
 * @brief Use a string key to lookup an element of a metadata map
 * node and return the entry value.
 *
 * @param[in] metadata The metadata node to query.
 *
 * @param[in] key A null terminated string that is the key to lookup.
 *
 * @param[out] value The metadata node of the @p key element of the
 * @p metadata map metadata node. The handle must be destroyed
 * using @c amd_comgr_destroy_metadata.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR The map has no entry
 * with a string key with the value @p key.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * metadata is an invalid metadata node, or not of kind @p
 * AMD_COMGR_METADATA_KIND_MAP. @p key or @p value is
 * NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to lookup metadata as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_metadata_lookup(
  amd_comgr_metadata_node_t metadata,
  const char *key,
  amd_comgr_metadata_node_t *value);

/**
 * @brief Get the list size from a metadata list node.
 *
 * @param[in] metadata The metadata node to query.
 *
 * @param[out] size The number of entries in the list.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * metadata is an invalid metadata node, or does nopt have kind @p
 * AMD_COMGR_METADATA_KIND_LIST. @p size is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_get_metadata_list_size(
  amd_comgr_metadata_node_t metadata,
  size_t *size);

/**
 * @brief Return the Nth metadata node of a list metadata node.
 *
 * @param[in] metadata The metadata node to query.
 *
 * @param[in] index The index being requested. The first list element
 * is index 0.
 *
 * @param[out] value The metadata node of the @p index element of the
 * @p metadata list metadata node. The handle must be destroyed
 * using @c amd_comgr_destroy_metadata.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p
 * metadata is an invalid metadata node or not of kind @p
 * AMD_COMGR_METADATA_INFO_LIST. @p index is greater
 * than the number of list elements. @p value is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to update action data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_index_list_metadata(
  amd_comgr_metadata_node_t metadata,
  size_t index,
  amd_comgr_metadata_node_t *value);

/**
 * @brief Iterate over the symbols of a machine code object.
 *
 * For a AMD_COMGR_DATA_KIND_RELOCATABLE the symbols in the ELF symtab section
 * are iterated. For a AMD_COMGR_DATA_KIND_EXECUTABLE the symbols in the ELF
 * dynsymtab are iterated.
 *
 * @param[in] data The data object to query.
 *
 * @param[in] callback The function to call for each symbol in the machine code
 * data object. The symbol handle is passed in @p symbol and @p user_data is
 * passed as @p user_data. If the function returns with a status other than @p
 * AMD_COMGR_STATUS_SUCCESS then iteration is stopped.
 *
 * @param[in] user_data The value to pass to each invocation of @p
 * callback. Allows context to be passed into the call back function.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR An error was
 * reported by @p callback.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p data is an invalid data
 * object, or not of kind @p AMD_COMGR_DATA_KIND_RELOCATABLE or
 * AMD_COMGR_DATA_KIND_EXECUTABLE. @p callback is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to iterate the data object as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_iterate_symbols(
  amd_comgr_data_t data,
  amd_comgr_status_t (*callback)(
    amd_comgr_symbol_t symbol,
    void *user_data),
  void *user_data);

/**
 * @brief Lookup a symbol in a machine code object by name.
 *
 * For a AMD_COMGR_DATA_KIND_RELOCATABLE the symbols in the ELF symtab section
 * are inspected. For a AMD_COMGR_DATA_KIND_EXECUTABLE the symbols in the ELF
 * dynsymtab are inspected.
 *
 * @param[in] data The data object to query.
 *
 * @param[in] name A null terminated string that is the symbol name to lookup.
 *
 * @param[out] symbol The symbol with the @p name.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR The machine code object has no symbol
 * with @p name.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p data is an invalid data
 * object, or not of kind @p AMD_COMGR_DATA_KIND_RELOCATABLE or
 * AMD_COMGR_DATA_KIND_EXECUTABLE.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to lookup symbol as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_symbol_lookup(
  amd_comgr_data_t data,
  const char *name,
  amd_comgr_symbol_t *symbol);

/**
 * @brief Machine code object symbol type.
 */
typedef enum amd_comgr_symbol_type_s {
  /**
   * The symbol's type is not specified.
  */
  AMD_COMGR_SYMBOL_TYPE_NOTYPE,

  /**
   * The symbol is associated with a data object, such as a variable, an array,
   * and so on.
  */
  AMD_COMGR_SYMBOL_TYPE_OBJECT,

  /**
   * The symbol is associated with a function or other executable code.
  */
  AMD_COMGR_SYMBOL_TYPE_FUNC,

  /**
   * The symbol is associated with a section. Symbol table entries of this type
   * exist primarily for relocation.
  */
  AMD_COMGR_SYMBOL_TYPE_SECTION,

  /**
   * Conventionally, the symbol's name gives the name of the source file
   * associated with the object file.
  */
  AMD_COMGR_SYMBOL_TYPE_FILE,

  /**
   * The symbol labels an uninitialized common block.
  */
  AMD_COMGR_SYMBOL_TYPE_COMMON
} amd_comgr_symbol_type_t;

/**
 * @brief Machine code object symbol attributes.
 */
typedef enum amd_comgr_symbol_info_s {
  /**
   * The length of the symbol name in bytes. Does not include the NUL
   * terminator. The type of this attribute is uint64_t.
  */
  AMD_COMGR_SYMBOL_INFO_NAME_LENGTH,

  /**
   * The name of the symbol. The type of this attribute is character array with
   * the length equal to the value of the @p AMD_COMGR_SYMBOL_INFO_NAME_LENGTH
   * attribute plus 1 for a NUL terminator.
  */
  AMD_COMGR_SYMBOL_INFO_NAME,

  /**
   * The kind of the symbol. The type of this attribute is @p
   * amd_comgr_symbol_type_t.
   */
  AMD_COMGR_SYMBOL_INFO_TYPE,

  /**
   * Size of the variable. The value of this attribute is undefined if the
   * symbol is not a variable. The type of this attribute is uint64_t.
   */
  AMD_COMGR_SYMBOL_INFO_SIZE,

  /**
   * Indicates whether the symbol is undefined. The type of this attribute is
   * bool.
   */
  AMD_COMGR_SYMBOL_INFO_IS_UNDEFINED,

  /**
   * The value of the symbol. The type of this attribute is uint64_t.
   */
  AMD_COMGR_SYMBOL_INFO_VALUE,

  /**
   * Marker for last valid symbol info.
   */
  AMD_COMGR_SYMBOL_INFO_LAST = AMD_COMGR_SYMBOL_INFO_VALUE
} amd_comgr_symbol_info_t;

/**
 * @brief Query information about a machine code object symbol.
 *
 * @param[in] symbol The symbol to query.
 *
 * @param[in] attribute Attribute to query.
 *
 * @param[out] value Pointer to an application-allocated buffer where to store
 * the value of the attribute. If the buffer passed by the application is not
 * large enough to hold the value of attribute, the behavior is undefined. The
 * type of value returned is specified by @p amd_comgr_symbol_info_t.
 *
 * @retval ::AMD_COMGR_STATUS_SUCCESS The function has
 * been executed successfully.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR The @p symbol does not have the requested @p
 * attribute.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_INVALID_ARGUMENT @p symbol is an invalid
 * symbol. @p attribute is an invalid value. @p value is NULL.
 *
 * @retval ::AMD_COMGR_STATUS_ERROR_OUT_OF_RESOURCES
 * Unable to query symbol as out of resources.
 */
amd_comgr_status_t AMD_API
amd_comgr_symbol_get_info(
  amd_comgr_symbol_t symbol,
  amd_comgr_symbol_info_t attribute,
  void *value);

/** @} */

#ifdef __cplusplus
}  /* end extern "C" block */
#endif

#endif  /* header guard */
