<?php

namespace Drupal\redhen_dedupe\Controller;

use Drupal\Core\Controller\ControllerBase;

/**
 * Default controller for the redhen_dedupe module.
 */
class DefaultController extends ControllerBase {

  /**
   * Create the listing page for duplicates.
   */
  public function redhenDedupeListPage() {
    $results = FALSE;
    $contacts = [];
    $base_fields = [];
    $fields = [];
    $active = TRUE;
    if (!isset($_POST['form_id'])) {
      if (isset($_GET['base_fields'])) {
        $base_fields = $_GET['base_fields'];
      }
      if (isset($_GET['fields'])) {
        $fields = $_GET['fields'];
      }
      if (empty($base_fields) && empty($fields)) {
        drupal_set_message(t('Please select at least one Field to match on.'), 'warning', FALSE);
      }
      if (!empty($base_fields) || !empty($fields)) {
        $active = isset($_GET['active']) ? $_GET['active'] : TRUE;
        $results = redhen_dedupe_get_duplicates($base_fields, $fields, $active);
      }
    }

    if (!empty($results)) {
      $message = t('The following sets of duplicate contacts have been found. Select the corresponding merge action to merge contact records.');
      $bundles = \Drupal::service('entity_type.bundle.info')->getBundleInfo('redhen_contact');
      $info = [];
      foreach (array_keys($bundles) as $bundle) {
        $info['base_field'] = \Drupal::service('entity_field.manager')->getBaseFieldDefinitions('redhen_contact', $bundle);
        $info['field'] = array_diff_key(\Drupal::service('entity_field.manager')->getFieldDefinitions('redhen_contact', $bundle), $info['base_field']);
      }

      $rows = [];
      $header = [];
      // Build our header array from the selected properties.
      foreach ($base_fields as $base_field) {
        $field_config = $info['base_field'][$base_field];
        $header[] = $field_config->getLabel();
      }

      foreach ($fields as $field) {
        $field_config = $info['field'][$field];
        $label = $field_config->getLabel();
        $header[] = $label;
      }
      $header[] = t('Count (IDs)');
      $header[] = '';
      // Display each result basing our row on the selected properties.
      foreach ($results as $result) {
        $ids = explode(',', $result->ids);
        // Dedupe by values:
        $ids = array_flip(array_flip($ids));
        if (count($ids) > 1) {
          $result->ids = implode(',', $ids);
          $col = [];
          foreach ($base_fields as $base_field) {
            $col[] = $result->{$base_field};
          }
          foreach ($fields as $field) {
            $field_config = $info['field'][$field];
            $name = $field_config->get('field_name');
            $columns = $field_config->getFieldStorageDefinition()->getColumns();
            reset($info['columns']);
            $name .= "_" . key($info['columns']);
            $col[] = $result->{$name};
          }
          $id_links = [];
          foreach ($ids as $id) {
            // TODO: Output html in the table correctly.
            $id_links[] = $id;
          }

          $count = $result->count . ' (' . implode(', ', $id_links) . ')';
          $col[] = $count;
          $url = \Drupal::service('path.validator')->getUrlIfValid('//admin/config/redhen/dedupe/merge/' . $result->ids);
          $col[] = $this->l(t('merge'), $url);

          $rows[] = $col;
        }
      }

      $contacts = [
        '#theme' => 'table',
        '#header' => $header,
        '#rows' => $rows,
      ];
    }
    else {
      $message = t('There are no duplicate contacts based on the selected properties. Expand your search or relax, you have no duplicates!');
    }

    return [
      'form' => \Drupal::formBuilder()->getForm('\Drupal\redhen_dedupe\Form\RedhenDedupeFilterForm', $base_fields, $fields, $active),
      'message' => [
        '#markup' => $message,
      ],
      'contacts' => $contacts,
    ];
  }

}
