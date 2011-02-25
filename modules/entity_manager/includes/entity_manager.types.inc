<?php
// $Id$

/**
 * @file
 */

/**
 * Menu callback: display an overview of available types.
 */
function entity_manager_types_overview($entity_type) {
//  drupal_add_css(drupal_get_path('module', 'commerce_product_ui') . '/theme/commerce_product_ui.types.css');

  $header = array(
    t('Name'),
    t('Operations'),
  );

  $rows = array();
  $entity_info = entity_get_info();

  // Loop through all defined bundle types.
  foreach (entity_manager_types($entity_type) as $type => $bundle_type) {
    // Build the operation links for the current bundle type.
    $links = menu_contextual_links(strtr($entity_type, array('_' => '-')), $entity_info[$entity_type]['managed options']['bundles']['path'], array(strtr($type, array('_' => '-'))));

    // Add the bundle type's row to the table's rows array.
    $rows[] = array(
      theme('entity_manager_type_admin_overview', array('bundle_type' => $bundle_type)),
      theme('links', array('links' => $links, 'attributes' => array('class' => 'links inline operations'))),
    );
  }

  // If no bundle types are defined...
  if (empty($rows)) {
    // Add a standard empty row with a link to add a new bundle type.
    $rows[] = array(
      array(
        'data' => t('There are no @label types yet. <a href="@link">Add @label type</a>.', array('@link' => url($entity_info[$entity_type]['managed options']['bundles']['path'] .'/add'), '@label' => $entity_info[$entity_type]['label'])),
        'colspan' => 2,
      )
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Builds an overview of a bundle type for display to an administrator.
 *
 * @param $variables
 *   An array of variables used to generate the display; by default includes the
 *     type key with a value of the bundle type object.
 *
 * @ingroup themeable
 */
function theme_entity_manager_type_admin_overview($variables) {
  $bundle_type = $variables['bundle_type'];

  $output = check_plain($bundle_type->name);
  $output .= ' <small> (Machine name: ' . check_plain($bundle_type->type) . ')</small>';
  $output .= '<div class="description">' . filter_xss_admin($bundle_type->description) . '</div>';

  return $output;
}

/**
 * Form callback wrapper: create or edit a bundle type.
 *
 * @param $bundle_type
 *   The bundle type object being edited by this form.
 *
 * @see entity_manager_bundle_type_form()
 */
function entity_manager_bundle_type_form_wrapper($bundle_type) {
  // Add the breadcrumb for the form's location.
//  commerce_product_ui_set_breadcrumb(TRUE);

  // Include the forms file from the contact module.
  module_load_include('inc', 'entity_manager', 'includes/entity_manager.forms');

  return drupal_get_form('entity_manager_bundle_type_form', $bundle_type);
}

/**
 * Form callback wrapper: confirmation form for deleting a bundle type.
 *
 * @param $bundle_type
 *   The bundle type object being deleted by this form.
 *
 * @see entity_manager_bundle_type_delete_form()
 */
function entity_manager_bundle_type_delete_form_wrapper($bundle_type) {
  // Add the breadcrumb for the form's location.
//  commerce_product_ui_set_breadcrumb(TRUE);

  // Don't allow deletion of bundle types that have entities already.
//  if (($count = db_query("SELECT cid FROM {crm_contact} WHERE type = :contact_type", array(':contact_type' => $bundle_type->type))->rowCount()) > 0) {
//    drupal_set_title(t('Cannot delete the %name contact type', array('%name' => $bundle_type->name)), PASS_THROUGH);
//    return t('There are @count contacts of this type. It cannot be deleted.', array('@count' => $count));
//  }

  // Include the forms file from the contact module.
  module_load_include('inc', 'entity_manager', 'includes/entity_manager.forms');

  return drupal_get_form('entity_manager_bundle_type_delete_form', $bundle_type);
}

/**
 * Form callback wrapper: confirmation form for reseting a bundle type.
 *
 * @param $bundle_type
 *   The bundle type object being deleted by this form.
 *
 * @see entity_manager_bundle_type_delete_form()
 */
function entity_manager_bundle_type_reset_form_wrapper($bundle_type) {
  // Add the breadcrumb for the form's location.
//  commerce_product_ui_set_breadcrumb(TRUE);

  // Include the forms file from the contact module.
  module_load_include('inc', 'entity_manager', 'includes/entity_manager.forms');

  return drupal_get_form('entity_manager_bundle_type_reset_form', $bundle_type);
}
