<?php
/**
 * @file
 * Field that indicates of a specific contact is the primary contact for an organization
 */
class redhen_relation_handler_field_primary_contact extends views_handler_field_boolean {

  function option_definition() {
    $options = parent::option_definition();
    $options['contact_relation'] = '';
    $options['org_relation'] = '';

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // get all relationships from the view
    $relationships = $this->view->display_handler->options['relationships'];
    // @todo parse the relationships to find valid options for contact and org. These aren't the relationship aliases that views uses later though.


    $form['contact_relation'] = array(
      '#type' => 'select',
      '#title' => t('Contact relationship'),
      '#description' => t('Select the relationship to use for the contact.'),
      '#default_value' => $this->options['contact_relation'],
      '#options' => $contact_options,
    );

    $form['org_relation'] = array(
      '#type' => 'select',
      '#title' => t('Organization relationship'),
      '#description' => t('Select the relationship to use for the organization.'),
      '#default_value' => $this->options['org_relation'],
      '#options' => $org_options,
    );

  }

  /**
   * Called to add the field to a query.
   */
  function query() {
    $this->ensure_my_table();

    $contact_table = $this->query->ensure_table('redhen_contact');
    $org_table = $this->query->ensure_table('redhen_org', $this->relationship);
    $comparison = 'primary_contact_id = ' . $contact_table . '.contact_id';
    $this->field_alias = $this->query->add_field(NULL, $comparison, 'is_primary');

    $this->add_additional_fields();
  }

}
