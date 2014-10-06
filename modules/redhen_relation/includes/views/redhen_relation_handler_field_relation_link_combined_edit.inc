<?php

/**
 * @file
 * Definition of redhen_relation_handler_field_relation_link_combined_edit.
 */

/**
 * Field handler to present a RedHen specific link relation edit.
 *
 * @ingroup views_field_handlers
 */
class redhen_relation_handler_field_relation_link_combined_edit extends views_handler_field_relation_link_edit {

  function option_definition() {
    $options = parent::option_definition();
    $options['redhen_entity_type'] = reset(redhen_entity_types());
    return $options;
  }

  function options_form(&$form, &$form_state) {
    // @TODO this could be prettied up.
    $form['redhen_entity_type'] = array(
      '#type' => 'select',
      '#title' => t('RedHen entity type to display form of'),
      '#default_value' => $this->options['redhen_entity_type'],
      '#options' => redhen_entity_types(),
    );
    parent::options_form($form, $form_state);
  }

  /**
   * Renders the link.
   */
  function render_link($relation, $values) {

    $types = redhen_entity_types();

    $type = $this->options['redhen_entity_type'];
    $id = $types[$type] . '_id';
    if (!$values->{$id} || !entity_access('update', 'relation', $relation)) {
      return;
    }

    $path = 'redhen/' . $types[$type] . '/' . $values->{$id} . '/connections/' . $relation->rid . '/edit';

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = $path;
    $this->options['alter']['query'] = drupal_get_destination();

    $text = !empty($this->options['text']) ? $this->options['text'] : t('Edit');
    return $text;
  }
}
