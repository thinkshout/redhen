<?php

/**
 * @file
 *
 * A Views' field handler for editing the RedHen relation affiliation status field.
 *
 */
class redhen_relation_handler_field_redhen_relation_status_edit extends views_handler_field {

  function construct() {
    parent::construct();
    $this->additional_fields['rid'] = 'rid';
    // @TODO Make this more abstract.
    $this->additional_fields[REDHEN_RELATION_STATUS_FIELD] = array(
      'table' => 'field_data_' . REDHEN_RELATION_STATUS_FIELD,
      'field' => REDHEN_RELATION_STATUS_FIELD . '_value'
    );
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * Render the field contents.
   *
   * @param $values
   * @return string
   */
  function render($values) {
    // Render a Views form item placeholder.
    return '<!--form-item-' . $this->options['id'] . '--' . $this->view->row_index . '-->';
  }

  /**
   * Add to and alter the form.
   */
  function views_form(&$form, &$form_redhen_relation_status) {
    // Create a container for our replacements
    $form[$this->options['id']] = array(
      '#type' => 'container',
      '#tree' => TRUE,
    );
    // Iterate over the result and add our replacement fields to the form.
    foreach($this->view->result as $row_index => $row) {
      // Add a text field to the form.  This array convention
      // corresponds to the placeholder HTML comment syntax.
      $form[$this->options['id']][$row_index] = array(
        '#type' => 'checkbox',
        '#default_value' => $row->{$this->aliases[REDHEN_RELATION_STATUS_FIELD]},
      );
    }
  }

  /**
   * Form submit method.
   */
  function views_form_submit($form, &$form_redhen_relation_status) {
    // Determine which relations we need to update.
    $updates = array();
    foreach($this->view->result as $row_index => $row) {
      $value = $form_redhen_relation_status['values'][$this->options['id']][$row_index];
      if ($row->{$this->aliases[REDHEN_RELATION_STATUS_FIELD]} != $value) {
        $updates[$row->{$this->aliases['rid']}] = $value;
      }
    }

    $relations = relation_load_multiple(array_keys($updates));
    foreach($relations as $rid => $relation) {
      $relation_wrapper = entity_metadata_wrapper('relation', $relation);
      $relation_wrapper->{REDHEN_RELATION_STATUS_FIELD}->set($updates[$rid]);
      $relation_wrapper->save();
    }

    $count = count($relations);
    if ($count) {
      drupal_set_message(
        format_plural(
          $count,
          'Updated 1 connection.',
          'Updated @count connections.'
        )
      );
    }
  }
}
