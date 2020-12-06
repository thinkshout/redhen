<?php

namespace Drupal\redhen_dedupe\Form;

use Drupal\Component\Utility\Xss;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;
use Drupal\Core\Template\Attribute;
use Drupal\redhen_connection\Entity\ConnectionType;
use Drupal\redhen_contact\Entity\Contact;

define('REDHEN_DEDUPE_NOT_APPLICABLE', 'redhen_dedupe_not_applicable');

/**
 * Form controller for Dedupe merge tool.
 *
 * @ingroup redhen_dedupe
 */
class RedhenDedupeMergeForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'redhen_dedupe_merge_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $entity_ids = NULL) {
    // Load the entities we want to merge:
    $entity_ids = explode(',', $entity_ids);
    $contacts = \Drupal::entityTypeManager()->getStorage('redhen_contact')->loadMultiple($entity_ids);
    $master_options = [];

    // Loop through the entities to build out our master entity options:
    foreach ($contacts as $ent_id => $entity) {
      $updated = format_date($entity->getChangedTime(), 'short');
      $master_options[$ent_id] = $this->t('@name (Updated: @date)', [
        '@date' => $updated,
        '@name' => $entity->label(),
      ]);
    }

    // Form field to select a merge master entity.
    $form['master'] = [
      '#type' => 'radios',
      '#title' => t('Master Contact'),
      '#default_value' => key($master_options),
      '#required' => TRUE,
      '#options' => $master_options,
      '#description' => $this->t('Choose a contact to merge the other contacts into.'),
      '#weight' => 0,
      '#ajax' => [
        'callback' => '\Drupal\redhen_dedupe\Form\RedhenDedupeMergeForm::redhenDedupeMergeFormCallback',
        'wrapper' => 'redhen_dedupe_merge_data',
      ],
    ];

    $merge_data_attributes = new Attribute();
    $merge_data_attributes['id'] = 'redhen_dedupe_merge_data';
    $form['merge_data'] = [
      '#type' => 'container',
      '#attributes' => $merge_data_attributes,
    ];

    $master_id = $form_state->getValue(['master']) ? (int) $form_state->getValue([
      'master',
    ]) : key($master_options);
    $merge_data = &$form['merge_data'];

    $view_builder = \Drupal::entityTypeManager()->getViewBuilder($entity->getEntityTypeId());
    $preview = $view_builder->view($contacts[$master_id]);
    $merge_data['contact_preview'] = [
      '#type' => 'fieldset',
      '#title' => $this->t('Master contact details'),
      'preview' => [
        '#markup' => render($preview),
      ],
    ];

    // Initialize our table header:
    $table_header = [$this->t('Field Name')];
    // Loop through the entities to build out our table headers and master
    // entity options:
    foreach ($contacts as $ent_id => $contact) {
      $updated = format_date($contact->getChangedTime(), 'short');
      $header_data = [
        '@date' => $updated,
        '@name' => $contact->label(),
        '@bundle' => $contact->bundle(),
        '@master' => ($ent_id == $master_id) ? t('Master') . ': ' : '',
      ];

      $table_header[$ent_id] = [
        'data' => $this->t('@master@name (@bundle)<br/>Last Updated: @date', $header_data),
        'class' => [($ent_id == $master_id) ? 'redhen-dedupe-master-col' : 'redhen-dedupe-col'],
      ];
    }

    // Pass along the entity ID options & master ID to the form handler:
    $form_state->set([
      'contacts',
    ], $contacts);

    // Now we build our merge selector form fields:
    $merge_data['values'] = [
      '#theme' => 'redhen_dedupe_form_table',
      '#tree' => TRUE,
      '#header' => $table_header,
    ];

    $bundles = \Drupal::service('entity_type.bundle.info')->getBundleInfo('redhen_contact');
    $info = [];
    foreach (array_keys($bundles) as $bundle) {
      $info[$bundle] = \Drupal::service('entity_field.manager')->getFieldDefinitions('redhen_contact', $bundle);
    }
    // Loop through each property and build a form element for it. The form
    // element will be placed into a table in redhen_dedupe_form_table:
    foreach ($info as $bundle => $field_definition) {
      foreach ($field_definition as $name => $field) {
        // Skip property if it does not exist on the master record.
        if (!isset($entity->{$name})) {
          continue;
        }

        // Call a helper function to determine we want to merge this field:
        if ($this->redhenDedupeBaseFieldMergeable($field)) {
          if ($this->redhenDedupeFieldIsMultivalue($field)) {
            $merge_data['values'][$name] = [
              '#type' => 'checkboxes',
              '#title' => Xss::filter($field->getLabel()),
              '#options' => [],
            ];
          }
          else {
            $merge_data['values'][$name] = [
              '#type' => 'radios',
              '#title' => $field->getLabel(),
              '#options' => [],
            ];
          }
          $options = &$merge_data['values'][$name]['#options'];
          // Loop through each contact to build a row element/radio button:
          foreach ($contacts as $ent_id => $contact) {
            // We do some work to figure out what kind of field we are dealing
            // with, and set our values and displays appropriately. The
            // important factors are if it's a field or not, and whether it has
            // a setter/getter callback that we should be using.
            $in_bundle = $bundle == $contact->bundle();
            if (!$in_bundle) {
              $options[$ent_id] = REDHEN_DEDUPE_NOT_APPLICABLE;
              continue;
            }

            // Set the default to match the Master record:
            if ($ent_id === $master_id) {
              $merge_data['values'][$name]['#default_value'] = $merge_data['values'][$name]['#type'] == 'radios' ? $ent_id : [
                $ent_id,
              ];
            }

            $options[$ent_id] = $this->redhenDedupeOptionLabel($contact, $name);
          }
        }
      }
    }

    // Exclude properties that are all the same from the merge form.
    foreach (Element::children($merge_data['values']) as $name) {
      $left = array_unique($merge_data['values'][$name]['#options']);
      // Filter out any remaining items that are not applicable.
      $left = array_filter($left, function ($item) {
        return ($item !== REDHEN_DEDUPE_NOT_APPLICABLE);
      });
      if (empty($left) || count($left) === 1) {
        unset($merge_data['values'][$name]);
        continue;
      }
    }

    $related_types = [];
    if (\Drupal::moduleHandler()->moduleExists('redhen_note')) {
      $related_types['redhen_note'] = t('Notes');
    }
    if (\Drupal::moduleHandler()->moduleExists('redhen_engagement')) {
      $related_types['redhen_engagement'] = t('Engagement Scores');
    }
    if (\Drupal::moduleHandler()->moduleExists('redhen_membership')) {
      $related_types['redhen_membership'] = t('Memberships');
    }
    if (\Drupal::moduleHandler()->moduleExists('redhen_connection')) {
      $related_types['redhen_connection'] = t('Connections/Affiliations');
    }
    if (count($related_types) > 0) {
      $form['related_entities'] = [
        '#type' => 'checkboxes',
        '#title' => t('Move items attached to old records to Master record:'),
        '#options' => $related_types,
        '#default_value' => array_keys($related_types),
      ];
    }

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Merge'),
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $master_id = $form_state->getValue(['master']);
    $contacts = $form_state->get(['contacts']);
    $master = $contacts[$master_id];
    $values = [];
    // Pull the actual data out of the #value array constructed for the form:
    if ($form_state->getValue(['values'])) {
      foreach ($form_state->getValue(['values']) as $name => $val) {
        if (is_array($val)) {
          $values[$name] = [
            'type' => 'combine',
            'value' => [],
          ];
          foreach ($val as $ent_id => $selected) {
            if ($selected) {
              $contact = Contact::load($ent_id);
              $values[$name]['value'][$ent_id] = $contact->get($name)->getValue();
            }
          }
        }
        else {
          $contact = $contacts[$val];
          $values[$name] = [
            'type' => 'direct',
            'value' => $contact->get($name)->getValue(),
          ];
        }
      }
    }
    unset($contacts[$master_id]);
    $related_entities = $form_state->getValue(['related_entities']);
    if (empty($related_entities)) {
      $related_entities = [];
    }
    else {
      $related_entities = array_filter($related_entities);
    }
    $merge_status = $this->redhenDedupeMerge($master, $values, $related_entities, $contacts);
    if ($merge_status) {
      $this->messenger()->addMessage(t('Contacts have successfully been merged into %master and deleted.', [
        '%master' => $master->label(),
      ]));
      $form_state->setRedirect('entity.redhen_contact.canonical',
       ['redhen_contact' => $master_id]);
    }
    else {
      $this->messenger()->addMessage(t('Error attempting to merge these contacts. Check the error log for more details.'), 'error');
    }
  }

  /**
   * Ajax callback for redhen_dedupe_merge_form().
   */
  public function redhenDedupeMergeFormCallback(array &$form, FormStateInterface $form_state) {
    $ajax_response = new AjaxResponse();

    $ajax_response->addCommand(new ReplaceCommand('#redhen_dedupe_merge_data', \Drupal::service('renderer')->render($form['merge_data'])));

    return $ajax_response;
  }

  /**
   * Determine if a given property can be merged.
   *
   * @param array $base_field
   *   A base_field as returned by
   *   \Drupal::service('entity_field.manager')
   *   ->getBaseFieldDefinitions('redhen_contact', $bundle).
   *
   * @return bool
   *   True is mergeable.
   */
  private function redhenDedupeBaseFieldMergeable($base_field) {
    // Don't merge computed fields:
    if ($base_field->isComputed()) {
      return FALSE;
    }

    // Don't merge fields that can't be edited:
    if ($base_field->isReadOnly()) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Return an option label for the merge form.
   *
   * @param Contact $contact
   *   Contact entity.
   * @param string $field_name
   *   Contact field name we need a label for.
   * @param array $field
   *   Full field config.
   *
   * @return string
   *   Label to use for an option field or other purpose.
   */
  private function redhenDedupeOptionLabel(Contact $contact, $field_name) {
    $render = $contact->get($field_name)->view(['label' => 'hidden']);
    $display = \Drupal::service('renderer')->render($render);
    return !$contact->get($field_name)->isEmpty() ? $display : $this->t('No value');
  }

  /**
   * Determine if a property should be merged via checkboxes instead of radios.
   */
  private function redhenDedupeFieldIsMultivalue($field) {
    $cardinality = $field->getFieldStorageDefinition()->getCardinality();
    if ($cardinality != 1) {
      return $cardinality;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Merge values from contacts into master contact and handle related entities.
   *
   * @param \Drupal\redhen_contact\Entity\Contact $master
   *   The master RedHen Contact.
   * @param array $values
   *   Values to update the master contact with.
   * @param array $related_entities
   *   Array of entity types to update to the master contact.
   * @param array $contacts
   *   The contacts being merged into the master.
   *
   * @return bool
   *   Result of the merge attempt.
   */
  private function redhenDedupeMerge(Contact $master, $values, $related_entities, $contacts = []) {
    $master_id = $master->id();

    $transaction = \Drupal::database()->startTransaction(__FUNCTION__);
    try {
      // Iterate through all contacts and update or delete related entities.
      foreach ($contacts as $contact) {
        $contact_id = $contact->id();

        // Update related entities:
        foreach ($related_entities as $entity_type) {
          switch ($entity_type) {
            case 'redhen_note':
            case 'redhen_engagement':
            case 'redhen_membership':
              // TODO redhen_notes, redhen_engagement, redhen_membership.
              $query = new EntityFieldQuery();
              $query->entityCondition('entity_type', $entity_type);
              $query->propertyCondition('entity_type', 'redhen_contact');
              $query->propertyCondition('entity_id', $contact_id);
              $result = $query->execute();
              if (!empty($result)) {
                $rel_entities = \Drupal::entityManager()->getStorage($entity_type);
                // Determine the property to change.
                $entity_key = ($entity_type == 'redhen_engagement') ? 'contact_id' : 'entity_id';
                foreach ($rel_entities as $rel_entity) {
                  $rel_entity->{$entity_key} = $master_id;
                  $rel_entity->save();
                }
              }
              break;

            case 'redhen_connection':
              // Look for connections w/ one end point including dupe contact.
              $results = \Drupal::service('redhen_connection.connections')->getConnections($contact);

              if ($results) {

                foreach ($results as $connection) {
                  $connection_type = ConnectionType::load($connection->bundle());
                  $contact_endpoint_fields = $connection_type->getEndpointFields('redhen_contact');
                  foreach ($contact_endpoint_fields as $contact_endpoint_field) {
                    // Iterate through endpoints and replace the endpoint that
                    // matches with the master contact.
                    if ($connection->get($contact_endpoint_field)->entity->id() == $contact_id) {
                      $connection->get($contact_endpoint_field)->setValue($master);
                    }
                  }
                  $connection->save();
                }
              }
              break;

            // @TODO entity_reference
            // case 'entity_reference'
          }
        }
      }

      // Delete old contacts.
      \Drupal::entityManager()->getStorage('redhen_contact')->delete($contacts);

      // Set the new values on the master contact.
      foreach ($values as $id => $value) {
        if ($value['type'] == 'direct') {
          $master->get($id)->setValue($value['value']);
        }
        if ($value['type'] == 'combine') {
          if (isset($value['value'][$master_id])) {
            // This assures that the "Master" record value is at the 0-index:
            $all_vals = $value['value'][$master_id];
            unset($value['value'][$master_id]);
          }
          else {
            $all_vals = [];
          }
          foreach ($value['value'] as $val) {
            $all_vals = array_merge($all_vals, $val);
          }
          if (!is_array(reset($all_vals)) && !is_object(reset($all_vals))) {
            $all_vals = array_unique($all_vals);
          }
          $master->get($id)->setValue($all_vals);
        }
      }

      $master->save();

      return TRUE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('redhen_dedupe', $e);
      return FALSE;
    }
  }

}
