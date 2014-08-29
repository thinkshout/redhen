<?php

/**
 * @file
 * Definition of redhen_org_views_handler_field_redhen_org_link.
 */

/**
 * Field handler to present the view link for a RedHen Organization.
 *
 * @ingroup views_field_handlers
 */
class redhen_org_views_handler_field_redhen_org_link extends views_handler_field_entity {

  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array('default' => '', 'translatable' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
    parent::options_form($form, $form_state);

    // The path is set by render_link function so don't allow to set it.
    $form['alter']['path'] = array('#access' => FALSE);
    $form['alter']['external'] = array('#access' => FALSE);
  }

  function render($values) {
    if ($entity = $this->get_value($values)) {
      return $this->render_link($entity, $values);
    }
  }

  function render_link($org, $values) {
    if (entity_access('view', 'redhen_org', $org)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = 'redhen/org/' . $org->org_id;
      $text = !empty($this->options['text']) ? $this->options['text'] : t('View');
      return $text;
    }
  }
}
